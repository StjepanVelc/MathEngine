window.CombinatoricsProbabilityStatisticsVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg", cache = new Map();
    const colors = { bar: "#a6e3a1", grid: "#45475a", text: "#cdd6f4" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }

    function histogram(containerId, bins) {
        const container = document.getElementById(containerId); container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 420", class: "combinatorics-probability-statistics-svg combinatorics-probability-statistics-draw", role: "img", "aria-label": "Histogram frekvencija" }); svg.append(node("title", {}, "Histogram frekvencija")); container.append(svg);
        const maxCount = Math.max(1, ...bins.map((bin) => bin.count));
        const originY = 380, plotLeft = 40, plotRight = 620, plotTop = 30;
        const barGap = 6;
        const barWidth = (plotRight - plotLeft) / bins.length - barGap;
        svg.append(node("line", { x1: plotLeft, y1: originY, x2: plotRight, y2: originY, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: plotLeft, y1: plotTop, x2: plotLeft, y2: originY, stroke: colors.text, "stroke-width": 2 }));
        bins.forEach((bin, index) => {
            const barHeight = (bin.count / maxCount) * (originY - plotTop);
            const x = plotLeft + index * (barWidth + barGap);
            svg.append(node("rect", { x, y: originY - barHeight, width: barWidth, height: barHeight, fill: colors.bar, rx: 3 }));
            svg.append(node("text", { x: x + barWidth / 2, y: originY + 18, "text-anchor": "middle", fill: colors.text, "font-size": 12 }, String(bin.count)));
        });
        cache.set(containerId, () => histogram(containerId, bins));
    }

    function refresh(panelId) { const visual = document.querySelector(`#${panelId} .combinatorics-probability-statistics-visual`); cache.get(visual?.id)?.(); }
    return { histogram, refresh };
})();
