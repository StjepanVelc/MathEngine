window.ExponentialLogarithmicVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg", cache = new Map();
    const colors = { curve: "#74c7ec", asymptote: "#f38ba8", grid: "#45475a", text: "#cdd6f4" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }

    function frame(containerId, title, samples) {
        const container = document.getElementById(containerId); container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 420", class: "exponential-logarithmic-svg exponential-logarithmic-draw", role: "img", "aria-label": title }); svg.append(node("title", {}, title)); container.append(svg);
        const finite = samples.filter((point) => Number.isFinite(point.x) && Number.isFinite(point.y));
        const xExtent = Math.max(1, ...finite.map((point) => Math.abs(point.x))) * 1.15;
        const yExtent = Math.max(1, ...finite.map((point) => Math.abs(point.y))) * 1.15;
        const origin = { x: 320, y: 210 };
        const scaleX = 280 / xExtent, scaleY = 170 / yExtent;
        const map = (point) => ({ x: origin.x + point.x * scaleX, y: origin.y - point.y * scaleY });
        for (let i = -4; i <= 4; i++) {
            const x = origin.x + (xExtent * i / 4) * scaleX, y = origin.y - (yExtent * i / 4) * scaleY;
            svg.append(node("line", { x1: x, y1: 24, x2: x, y2: 396, stroke: colors.grid, opacity: .35 }));
            svg.append(node("line", { x1: 24, y1: y, x2: 616, y2: y, stroke: colors.grid, opacity: .35 }));
        }
        svg.append(node("line", { x1: 20, y1: origin.y, x2: 620, y2: origin.y, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: origin.x, y1: 400, x2: origin.x, y2: 20, stroke: colors.text, "stroke-width": 2 }));
        return { svg, map, origin };
    }

    function polyline(svg, map, points, color) {
        const valid = points.filter((point) => Number.isFinite(point.x) && Number.isFinite(point.y));
        svg.append(node("polyline", { points: valid.map((point) => { const p = map(point); return `${p.x},${p.y}`; }).join(" "), fill: "none", stroke: color, "stroke-width": 3, "stroke-linejoin": "round" }));
    }

    function exponential(containerId, samples) {
        const context = frame(containerId, "Graf eksponencijalne funkcije", samples);
        context.svg.append(node("line", { x1: 20, y1: context.origin.y, x2: 620, y2: context.origin.y, stroke: colors.asymptote, "stroke-dasharray": "6 5", opacity: .6 }));
        polyline(context.svg, context.map, samples, colors.curve);
        cache.set(containerId, () => exponential(containerId, samples));
    }

    function logarithm(containerId, samples) {
        const context = frame(containerId, "Graf logaritamske funkcije", samples);
        context.svg.append(node("line", { x1: context.origin.x, y1: 20, x2: context.origin.x, y2: 400, stroke: colors.asymptote, "stroke-dasharray": "6 5", opacity: .6 }));
        polyline(context.svg, context.map, samples, colors.curve);
        cache.set(containerId, () => logarithm(containerId, samples));
    }

    function refresh(panelId) { const visual = document.querySelector(`#${panelId} .exponential-logarithmic-visual`); cache.get(visual?.id)?.(); }
    return { exponential, logarithm, refresh };
})();
