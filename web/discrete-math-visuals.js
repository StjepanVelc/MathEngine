window.DiscreteMathVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg", cache = new Map();
    const colors = { a: "#89dceb77", b: "#f38ba877", both: "#a6e3a199", edge: "#585b70", node: "#89b4fa", nodeText: "#11111b", path: "#a6e3a1", text: "#cdd6f4", axis: "#585b70", bar: "#89b4fa" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }
    function svgFor(containerId, label) {
        const container = document.getElementById(containerId); if (!container) return null; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 380", class: "discrete-math-svg", role: "img", "aria-label": label });
        svg.append(node("title", {}, label)); container.append(svg); return svg;
    }
    function parseSet(text) { return [...new Set(text.split(",").map((s) => s.trim()).filter(Boolean))]; }
    function parseAdjacency(text) { return text.split(";").map((part) => part.trim() ? part.split(",").map((s) => Number(s.trim())) : []); }

    function setOperation(containerId, setA, setB, mode) {
        const svg = svgFor(containerId, "Vennov dijagram operacije nad skupovima"); if (!svg) return;
        const a = parseSet(setA), b = parseSet(setB);
        const onlyA = a.filter((x) => !b.includes(x));
        const onlyB = b.filter((x) => !a.includes(x));
        const shared = a.filter((x) => b.includes(x));
        const highlight = mode === "union" ? [...onlyA, ...shared, ...onlyB]
            : mode === "intersection" ? shared
            : mode === "difference" ? onlyA
            : [...onlyA, ...onlyB];
        const cx1 = 250, cx2 = 390, cy = 190, r = 130;
        function fillFor(inA, inB) {
            const el = inA && inB ? "both" : inA ? "a" : "b";
            const included = mode === "union" ? true
                : mode === "intersection" ? (inA && inB)
                : mode === "difference" ? (inA && !inB)
                : (inA !== inB);
            return included ? colors[el] : "#2a2f3e55";
        }
        svg.append(node("circle", { cx: cx1, cy, r, fill: fillFor(true, false), stroke: colors.a, "stroke-width": 2 }));
        svg.append(node("circle", { cx: cx2, cy, r, fill: fillFor(false, true), stroke: colors.b, "stroke-width": 2 }));
        svg.append(node("text", { x: cx1 - r, y: cy - r - 12, fill: colors.text, "font-size": 18, "font-weight": 700 }, "A"));
        svg.append(node("text", { x: cx2 + r - 14, y: cy - r - 12, fill: colors.text, "font-size": 18, "font-weight": 700 }, "B"));
        function place(items, xCenter) {
            const cols = Math.ceil(Math.sqrt(items.length || 1));
            items.forEach((value, index) => {
                const row = Math.floor(index / cols), col = index % cols;
                const x = xCenter + (col - (cols - 1) / 2) * 34;
                const y = cy + (row - (Math.ceil(items.length / cols) - 1) / 2) * 28;
                svg.append(node("text", { x, y, fill: colors.text, "font-size": 15, "text-anchor": "middle", "font-weight": highlight.includes(value) ? 800 : 400 }, String(value)));
            });
        }
        place(onlyA, cx1 - 55);
        place(shared, (cx1 + cx2) / 2);
        place(onlyB, cx2 + 55);
        cache.set(containerId, () => setOperation(containerId, setA, setB, mode));
    }

    function circularLayout(count, cx, cy, r) {
        const positions = [];
        for (let i = 0; i < count; i++) { const angle = (2 * Math.PI * i) / count - Math.PI / 2; positions.push([cx + r * Math.cos(angle), cy + r * Math.sin(angle)]); }
        return positions;
    }

    function drawGraphBase(svg, adjacency, directed, positions, edgeColor = colors.edge, highlightEdges = new Set()) {
        adjacency.forEach((neighbors, from) => {
            neighbors.forEach((to) => {
                if (directed || from < to) {
                    const key = `${from}-${to}`, altKey = `${to}-${from}`;
                    const isHighlighted = highlightEdges.has(key) || (!directed && highlightEdges.has(altKey));
                    const [x1, y1] = positions[from], [x2, y2] = positions[to];
                    svg.append(node("line", { x1, y1, x2, y2, stroke: isHighlighted ? colors.path : edgeColor, "stroke-width": isHighlighted ? 4 : 2, "marker-end": directed ? "url(#dm-arrow)" : undefined }));
                }
            });
        });
        positions.forEach(([x, y], index) => {
            const isHighlighted = [...highlightEdges].some((key) => key.split("-").map(Number).includes(index));
            svg.append(node("circle", { cx: x, cy: y, r: 16, fill: isHighlighted ? colors.path : colors.node, stroke: colors.text, "stroke-width": 1.5 }));
            svg.append(node("text", { x, y: y + 5, fill: colors.nodeText, "font-size": 14, "text-anchor": "middle", "font-weight": 700 }, String(index)));
        });
    }

    function addArrowMarker(svg) {
        const defs = node("defs");
        const marker = node("marker", { id: "dm-arrow", viewBox: "0 0 10 10", refX: 24, refY: 5, markerWidth: 6, markerHeight: 6, orient: "auto-start-reverse" });
        marker.append(node("path", { d: "M0,0 L10,5 L0,10 z", fill: colors.edge }));
        defs.append(marker); svg.append(defs);
    }

    function graphDiagram(containerId, adjacencyText, directed) {
        const svg = svgFor(containerId, "Prikaz grafa"); if (!svg) return;
        const adjacency = parseAdjacency(adjacencyText);
        if (directed) addArrowMarker(svg);
        const positions = circularLayout(adjacency.length, 320, 190, 140);
        drawGraphBase(svg, adjacency, directed, positions);
        cache.set(containerId, () => graphDiagram(containerId, adjacencyText, directed));
    }

    function pathDiagram(containerId, adjacencyText, path) {
        const svg = svgFor(containerId, "Najkraci put u grafu"); if (!svg) return;
        const adjacency = parseAdjacency(adjacencyText);
        const positions = circularLayout(adjacency.length, 320, 190, 140);
        const highlightEdges = new Set();
        for (let i = 0; i < path.length - 1; i++) highlightEdges.add(`${path[i]}-${path[i + 1]}`);
        drawGraphBase(svg, adjacency, false, positions, colors.edge, highlightEdges);
        cache.set(containerId, () => pathDiagram(containerId, adjacencyText, path));
    }

    function termsChart(containerId, terms) {
        const svg = svgFor(containerId, "Clanovi niza"); if (!svg) return;
        const plotLeft = 50, plotRight = 610, plotTop = 30, plotBottom = 340;
        const minY = Math.min(0, ...terms), maxY = Math.max(0, ...terms);
        const spanY = (maxY - minY) || 1;
        const toY = (y) => plotBottom - ((y - minY) / spanY) * (plotBottom - plotTop);
        const barWidth = (plotRight - plotLeft) / terms.length * 0.6;
        svg.append(node("line", { x1: plotLeft, y1: toY(0), x2: plotRight, y2: toY(0), stroke: colors.axis, "stroke-width": 2 }));
        terms.forEach((value, index) => {
            const x = plotLeft + ((index + 0.5) / terms.length) * (plotRight - plotLeft);
            const y = toY(value), zero = toY(0);
            svg.append(node("rect", { x: x - barWidth / 2, y: Math.min(y, zero), width: barWidth, height: Math.abs(y - zero) || 1, fill: colors.bar }));
            svg.append(node("text", { x, y: y - (value >= 0 ? 8 : -18), fill: colors.text, "font-size": 12, "text-anchor": "middle" }, `a${index}=${Number(value.toFixed(2))}`));
        });
        cache.set(containerId, () => termsChart(containerId, terms));
    }

    function inclusionExclusion(containerId, sizeA, sizeB, sizeAB) {
        const svg = svgFor(containerId, "Vennov dijagram unije skupova"); if (!svg) return;
        const cx1 = 250, cx2 = 390, cy = 190;
        const scale = 6, r1 = Math.max(50, Math.sqrt(Math.max(sizeA, 1)) * scale * 6), r2 = Math.max(50, Math.sqrt(Math.max(sizeB, 1)) * scale * 6);
        svg.append(node("circle", { cx: cx1, cy, r: r1, fill: colors.a, stroke: "#89dceb", "stroke-width": 2 }));
        svg.append(node("circle", { cx: cx2, cy, r: r2, fill: colors.b, stroke: "#f38ba8", "stroke-width": 2 }));
        svg.append(node("text", { x: cx1 - r1 * 0.6, y: cy, fill: colors.text, "font-size": 16, "font-weight": 700, "text-anchor": "middle" }, `|A|=${sizeA}`));
        svg.append(node("text", { x: cx2 + r2 * 0.6, y: cy, fill: colors.text, "font-size": 16, "font-weight": 700, "text-anchor": "middle" }, `|B|=${sizeB}`));
        svg.append(node("text", { x: (cx1 + cx2) / 2, y: cy, fill: colors.text, "font-size": 16, "font-weight": 700, "text-anchor": "middle" }, `${sizeAB}`));
        svg.append(node("text", { x: (cx1 + cx2) / 2, y: cy - Math.max(r1, r2) - 16, fill: colors.text, "font-size": 15, "text-anchor": "middle" }, `|A ∪ B| = ${sizeA + sizeB - sizeAB}`));
        cache.set(containerId, () => inclusionExclusion(containerId, sizeA, sizeB, sizeAB));
    }

    function relationGraph(containerId, domainText, pairsText) {
        const svg = svgFor(containerId, "Graf relacije"); if (!svg) return;
        const domain = parseSet(domainText);
        addArrowMarker(svg);
        const positions = circularLayout(domain.length, 320, 190, 140);
        const indexOf = (value) => domain.indexOf(value);
        pairsText.split(";").map((p) => p.trim()).filter(Boolean).forEach((pair) => {
            const [from, to] = pair.split("-").map((s) => s.trim());
            const i = indexOf(from), j = indexOf(to);
            if (i < 0 || j < 0) return;
            if (i === j) {
                const [x, y] = positions[i];
                svg.append(node("circle", { cx: x, cy: y - 28, r: 14, fill: "none", stroke: colors.edge, "stroke-width": 2 }));
            } else {
                const [x1, y1] = positions[i], [x2, y2] = positions[j];
                svg.append(node("line", { x1, y1, x2, y2, stroke: colors.edge, "stroke-width": 2, "marker-end": "url(#dm-arrow)" }));
            }
        });
        positions.forEach(([x, y], index) => {
            svg.append(node("circle", { cx: x, cy: y, r: 16, fill: colors.node, stroke: colors.text, "stroke-width": 1.5 }));
            svg.append(node("text", { x, y: y + 5, fill: colors.nodeText, "font-size": 13, "text-anchor": "middle", "font-weight": 700 }, String(domain[index])));
        });
        cache.set(containerId, () => relationGraph(containerId, domainText, pairsText));
    }

    function refresh(panelId) { const visual = document.querySelector(`#${panelId} .discrete-math-visual`); cache.get(visual?.id)?.(); }
    return { setOperation, graphDiagram, pathDiagram, termsChart, inclusionExclusion, relationGraph, refresh };
})();
