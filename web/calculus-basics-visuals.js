window.CalculusBasicsVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg", cache = new Map();
    const colors = { line: "#89dceb", point: "#a6e3a1", target: "#f38ba8", grid: "#45475a", text: "#cdd6f4" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }

    function limitApproach(containerId, point, limitValue, leftSamples, rightSamples) {
        const container = document.getElementById(containerId); if (!container) return; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 420", class: "calculus-basics-svg calculus-basics-draw", role: "img", "aria-label": "Priblizavanje limesu" }); svg.append(node("title", {}, "Priblizavanje limesu")); container.append(svg);
        const samples = [...leftSamples, ...rightSamples];
        const values = [...samples.map((sample) => sample.value), limitValue];
        const xs = [...samples.map((sample) => sample.x), point];
        const minX = Math.min(...xs), maxX = Math.max(...xs);
        const minY = Math.min(...values), maxY = Math.max(...values);
        const spanX = maxX - minX || 1, spanY = maxY - minY || 1;
        const plotLeft = 50, plotRight = 610, plotTop = 30, plotBottom = 380;
        const toX = (x) => plotLeft + ((x - minX) / spanX) * (plotRight - plotLeft);
        const toY = (y) => plotBottom - ((y - minY) / spanY) * (plotBottom - plotTop);
        svg.append(node("line", { x1: plotLeft, y1: plotBottom, x2: plotRight, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: plotLeft, y1: plotTop, x2: plotLeft, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        [leftSamples, rightSamples].forEach((group) => {
            group.forEach((sample) => svg.append(node("circle", { cx: toX(sample.x), cy: toY(sample.value), r: 5, fill: colors.point })));
        });
        svg.append(node("circle", { cx: toX(point), cy: toY(limitValue), r: 7, fill: colors.target }));
        svg.append(node("text", { x: toX(point) + 10, y: toY(limitValue) - 10, fill: colors.text, "font-size": 13 }, `L = ${Number(limitValue.toFixed(4))}`));
        cache.set(containerId, () => limitApproach(containerId, point, limitValue, leftSamples, rightSamples));
    }

    function refresh(panelId) { const visual = document.querySelector(`#${panelId} .calculus-basics-visual`); cache.get(visual?.id)?.(); }
    return { limitApproach, refresh };
})();
