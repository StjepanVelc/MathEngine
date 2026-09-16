window.ProbabilityStatisticsVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg";
    const colors = { axis: "#585b70", curve: "#89b4fa", fill: "#89dceb33", point: "#a6e3a1", line: "#f38ba8", text: "#cdd6f4" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }
    function svgFor(containerId, label) {
        const container = document.getElementById(containerId); if (!container) return null; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 380", class: "probability-statistics-svg", role: "img", "aria-label": label });
        svg.append(node("title", {}, label)); container.append(svg); return svg;
    }

    function normalCurve(containerId, mean, stdDev) {
        const svg = svgFor(containerId, "Graf gustoce normalne razdiobe"); if (!svg) return;
        const width = 640, height = 380, marginX = 50, marginY = 40;
        const plotWidth = width - 2 * marginX, plotHeight = height - 2 * marginY;
        const xMin = mean - 4 * stdDev, xMax = mean + 4 * stdDev;
        const density = (x) => Math.exp(-0.5 * ((x - mean) / stdDev) ** 2) / (stdDev * Math.sqrt(2 * Math.PI));
        const yMax = density(mean);
        const toX = (x) => marginX + ((x - xMin) / (xMax - xMin)) * plotWidth;
        const toY = (y) => marginY + plotHeight - (y / yMax) * plotHeight;

        svg.append(node("line", { x1: marginX, y1: marginY + plotHeight, x2: marginX + plotWidth, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));
        svg.append(node("line", { x1: marginX, y1: marginY, x2: marginX, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));

        const steps = 120;
        const points = [];
        for (let i = 0; i <= steps; i++) {
            const x = xMin + ((xMax - xMin) * i) / steps;
            points.push([toX(x), toY(density(x))]);
        }
        const pathData = points.map(([x, y], i) => `${i === 0 ? "M" : "L"}${x.toFixed(2)},${y.toFixed(2)}`).join(" ");
        const areaData = `${pathData} L${toX(xMax).toFixed(2)},${(marginY + plotHeight).toFixed(2)} L${toX(xMin).toFixed(2)},${(marginY + plotHeight).toFixed(2)} Z`;
        svg.append(node("path", { d: areaData, fill: colors.fill, stroke: "none" }));
        svg.append(node("path", { d: pathData, fill: "none", stroke: colors.curve, "stroke-width": 2.5 }));

        const meanX = toX(mean);
        svg.append(node("line", { x1: meanX, y1: marginY, x2: meanX, y2: marginY + plotHeight, stroke: colors.line, "stroke-width": 1.5, "stroke-dasharray": "4 4" }));
        svg.append(node("text", { x: meanX, y: marginY - 10, fill: colors.text, "font-size": 13, "text-anchor": "middle" }, `mu = ${formatNumberForDisplay(mean, 2)}`));
        [-2, -1, 0, 1, 2].forEach((k) => {
            const x = mean + k * stdDev;
            svg.append(node("text", { x: toX(x), y: marginY + plotHeight + 22, fill: colors.text, "font-size": 12, "text-anchor": "middle" }, formatNumberForDisplay(x, 2)));
        });
    }

    function uniformDensity(containerId, lowerBound, upperBound) {
        const svg = svgFor(containerId, "Graf gustoce uniformne razdiobe"); if (!svg) return;
        const width = 640, height = 380, marginX = 50, marginY = 40;
        const plotWidth = width - 2 * marginX, plotHeight = height - 2 * marginY;
        const span = upperBound - lowerBound;
        const xMin = lowerBound - span * 0.3, xMax = upperBound + span * 0.3;
        const density = 1 / span;
        const yMax = density * 1.3;
        const toX = (x) => marginX + ((x - xMin) / (xMax - xMin)) * plotWidth;
        const toY = (y) => marginY + plotHeight - (y / yMax) * plotHeight;

        svg.append(node("line", { x1: marginX, y1: marginY + plotHeight, x2: marginX + plotWidth, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));
        svg.append(node("line", { x1: marginX, y1: marginY, x2: marginX, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));

        const baseline = marginY + plotHeight;
        const pathData = `M${toX(xMin).toFixed(2)},${baseline} L${toX(lowerBound).toFixed(2)},${baseline} L${toX(lowerBound).toFixed(2)},${toY(density).toFixed(2)} L${toX(upperBound).toFixed(2)},${toY(density).toFixed(2)} L${toX(upperBound).toFixed(2)},${baseline} L${toX(xMax).toFixed(2)},${baseline}`;
        svg.append(node("path", { d: pathData, fill: "none", stroke: colors.curve, "stroke-width": 2.5 }));
        svg.append(node("text", { x: toX(lowerBound), y: marginY + plotHeight + 22, fill: colors.text, "font-size": 12, "text-anchor": "middle" }, `a=${formatNumberForDisplay(lowerBound, 2)}`));
        svg.append(node("text", { x: toX(upperBound), y: marginY + plotHeight + 22, fill: colors.text, "font-size": 12, "text-anchor": "middle" }, `b=${formatNumberForDisplay(upperBound, 2)}`));
    }

    function regressionScatter(containerId, x, y, slope, intercept) {
        const svg = svgFor(containerId, "Graf tocaka i regresijskog pravca"); if (!svg) return;
        const width = 640, height = 380, marginX = 50, marginY = 40;
        const plotWidth = width - 2 * marginX, plotHeight = height - 2 * marginY;
        const xMin = Math.min(...x), xMax = Math.max(...x);
        const yMin = Math.min(...y, ...x.map((xi) => slope * xi + intercept));
        const yMax = Math.max(...y, ...x.map((xi) => slope * xi + intercept));
        const xPad = (xMax - xMin || 1) * 0.15, yPad = (yMax - yMin || 1) * 0.15;
        const toX = (v) => marginX + ((v - (xMin - xPad)) / ((xMax + xPad) - (xMin - xPad))) * plotWidth;
        const toY = (v) => marginY + plotHeight - ((v - (yMin - yPad)) / ((yMax + yPad) - (yMin - yPad))) * plotHeight;

        svg.append(node("line", { x1: marginX, y1: marginY + plotHeight, x2: marginX + plotWidth, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));
        svg.append(node("line", { x1: marginX, y1: marginY, x2: marginX, y2: marginY + plotHeight, stroke: colors.axis, "stroke-width": 2 }));

        const lineX1 = xMin - xPad, lineX2 = xMax + xPad;
        svg.append(node("line", {
            x1: toX(lineX1), y1: toY(slope * lineX1 + intercept),
            x2: toX(lineX2), y2: toY(slope * lineX2 + intercept),
            stroke: colors.line, "stroke-width": 2.5
        }));

        x.forEach((xi, index) => {
            svg.append(node("circle", { cx: toX(xi), cy: toY(y[index]), r: 5, fill: colors.point }));
        });
    }

    return { normalCurve, uniformDensity, regressionScatter };
})();
