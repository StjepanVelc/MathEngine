window.ComplexNumbersVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg";
    const colors = { axis: "#585b70", grid: "#45475a55", a: "#89b4fa", b: "#f38ba8", result: "#a6e3a1", text: "#cdd6f4" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }
    function svgFor(containerId, label) {
        const container = document.getElementById(containerId); if (!container) return null; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 380", class: "complex-numbers-svg", role: "img", "aria-label": label });
        svg.append(node("title", {}, label)); container.append(svg); return svg;
    }

    function drawAxes(svg, toX, toY, xMin, xMax, yMin, yMax) {
        for (let x = Math.ceil(xMin); x <= Math.floor(xMax); x++) {
            if (x === 0) continue;
            svg.append(node("line", { x1: toX(x), y1: toY(yMin), x2: toX(x), y2: toY(yMax), stroke: colors.grid, "stroke-width": 1 }));
        }
        for (let y = Math.ceil(yMin); y <= Math.floor(yMax); y++) {
            if (y === 0) continue;
            svg.append(node("line", { x1: toX(xMin), y1: toY(y), x2: toX(xMax), y2: toY(y), stroke: colors.grid, "stroke-width": 1 }));
        }
        svg.append(node("line", { x1: toX(xMin), y1: toY(0), x2: toX(xMax), y2: toY(0), stroke: colors.axis, "stroke-width": 2 }));
        svg.append(node("line", { x1: toX(0), y1: toY(yMin), x2: toX(0), y2: toY(yMax), stroke: colors.axis, "stroke-width": 2 }));
        svg.append(node("text", { x: toX(xMax) - 8, y: toY(0) - 8, fill: colors.text, "font-size": 13, "text-anchor": "end" }, "Re"));
        svg.append(node("text", { x: toX(0) + 10, y: toY(yMax) + 14, fill: colors.text, "font-size": 13 }, "Im"));
    }

    function makeScale(points) {
        const width = 640, height = 380, margin = 40;
        const plotWidth = width - 2 * margin, plotHeight = height - 2 * margin;
        const reals = points.map((p) => p.real).concat([0]);
        const imags = points.map((p) => p.imaginary).concat([0]);
        const spanX = Math.max(...reals) - Math.min(...reals) || 2;
        const spanY = Math.max(...imags) - Math.min(...imags) || 2;
        const xMin = Math.min(...reals) - spanX * 0.35 - 0.5, xMax = Math.max(...reals) + spanX * 0.35 + 0.5;
        const yMin = Math.min(...imags) - spanY * 0.35 - 0.5, yMax = Math.max(...imags) + spanY * 0.35 + 0.5;
        const toX = (re) => margin + ((re - xMin) / (xMax - xMin)) * plotWidth;
        const toY = (im) => margin + plotHeight - ((im - yMin) / (yMax - yMin)) * plotHeight;
        return { toX, toY, xMin, xMax, yMin, yMax };
    }

    function plotPoints(containerId, points, label) {
        const svg = svgFor(containerId, label || "Argandov dijagram kompleksnih brojeva"); if (!svg) return;
        const { toX, toY, xMin, xMax, yMin, yMax } = makeScale(points.map((p) => p.value));
        drawAxes(svg, toX, toY, xMin, xMax, yMin, yMax);

        points.forEach(({ value, color, label: pointLabel }) => {
            const x0 = toX(0), y0 = toY(0), x1 = toX(value.real), y1 = toY(value.imaginary);
            svg.append(node("line", { x1: x0, y1: y0, x2: x1, y2: y1, stroke: color, "stroke-width": 2.5, "marker-end": "" }));
            svg.append(node("circle", { cx: x1, cy: y1, r: 5, fill: color }));
            if (pointLabel) svg.append(node("text", { x: x1 + 10, y: y1 - 10, fill: colors.text, "font-size": 13, "font-weight": 700 }, pointLabel));
        });
    }

    function operation(containerId, a, b, result) {
        plotPoints(containerId, [
            { value: a, color: colors.a, label: "z1" },
            { value: b, color: colors.b, label: "z2" },
            { value: result, color: colors.result, label: "rezultat" }
        ]);
    }

    function singleValue(containerId, value, label) {
        plotPoints(containerId, [{ value, color: colors.result, label: label || "z" }]);
    }

    function roots(containerId, rootsList) {
        plotPoints(containerId, rootsList.map((value, index) => ({ value, color: index === 0 ? colors.result : colors.a, label: `w${index}` })), "Argandov dijagram n-tih korijena");
    }

    function quadraticRoots(containerId, root1, root2) {
        plotPoints(containerId, [
            { value: root1, color: colors.a, label: "x1" },
            { value: root2, color: colors.b, label: "x2" }
        ], "Argandov dijagram rjesenja kvadratne jednadzbe");
    }

    return { operation, singleValue, roots, quadraticRoots };
})();
