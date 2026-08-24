window.SequencesVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg";
    const cache = new Map();
    const colors = { term: "#a6e3a1", sum: "#89b4fa", accent: "#fab387", grid: "#45475a", text: "#cdd6f4", muted: "#7f849c" };

    function node(name, attributes = {}, text = "") {
        const element = document.createElementNS(ns, name);
        Object.entries(attributes).forEach(([key, value]) => element.setAttribute(key, value));
        if (text) element.textContent = text;
        return element;
    }
    function finite(values) { return values.map(Number).filter(Number.isFinite); }
    function scale(values, min, max, top, bottom) {
        const low = Math.min(...values), high = Math.max(...values);
        const span = high - low || 1;
        return (value) => bottom - ((value - low) / span) * (bottom - top);
    }
    function shell(containerId, title) {
        const container = document.getElementById(containerId);
        container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 390", class: "sequences-svg sequences-draw", role: "img", "aria-label": title });
        svg.append(node("title", {}, title));
        container.append(svg); return svg;
    }
    function axes(svg, left = 58, right = 610, top = 45, bottom = 330) {
        for (let i = 0; i <= 5; i++) {
            const y = top + i * (bottom - top) / 5;
            svg.append(node("line", { x1: left, y1: y, x2: right, y2: y, stroke: colors.grid, "stroke-width": 1, opacity: .65 }));
        }
        svg.append(node("line", { x1: left, y1: bottom, x2: right, y2: bottom, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: left, y1: top, x2: left, y2: bottom, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("text", { x: right, y: bottom + 28, fill: colors.muted, "text-anchor": "end" }, "indeks n"));
    }
    function plot(svg, values, options = {}) {
        const data = finite(values); if (!data.length) return;
        const left = 58, right = 610, top = 55, bottom = 330;
        const y = scale(options.scaleValues || data, 0, 0, top, bottom);
        const x = (index) => data.length === 1 ? (left + right) / 2 : left + index * (right - left) / (data.length - 1);
        if (options.connect !== false) {
            const points = data.map((value, index) => `${x(index)},${y(value)}`).join(" ");
            svg.append(node("polyline", { points, fill: "none", stroke: options.color || colors.term, "stroke-width": 3, "stroke-linejoin": "round", opacity: .8 }));
        }
        data.forEach((value, index) => {
            const group = node("g", { class: "sequences-pop", style: `animation-delay:${Math.min(index * 45, 450)}ms` });
            group.append(node("line", { x1: x(index), y1: bottom, x2: x(index), y2: y(value), stroke: options.color || colors.term, opacity: .22 }));
            group.append(node("circle", { cx: x(index), cy: y(value), r: 5.5, fill: options.color || colors.term, stroke: "#181825", "stroke-width": 2 }));
            if (data.length <= 12) group.append(node("text", { x: x(index), y: bottom + 19, fill: colors.muted, "text-anchor": "middle", "font-size": 11 }, String(index + 1)));
            svg.append(group);
        });
        return { y, x, top, bottom };
    }
    function legend(svg, items) {
        items.forEach(([label, color], index) => {
            const x = 65 + index * 160;
            svg.append(node("circle", { cx: x, cy: 24, r: 5, fill: color }));
            svg.append(node("text", { x: x + 11, y: 28, fill: colors.text, "font-size": 13 }, label));
        });
    }

    function points(containerId, terms, options = {}) {
        const values = finite(terms); const svg = shell(containerId, options.title || "Diskretni graf niza");
        axes(svg); legend(svg, [["član aₙ", colors.term]]); plot(svg, values);
        cache.set(containerId, () => points(containerId, terms, options));
    }
    function combined(containerId, terms, sums, options = {}) {
        const termValues = finite(terms), sumValues = finite(sums);
        const svg = shell(containerId, options.title || "Članovi i parcijalne sume");
        axes(svg); legend(svg, [["član aₙ", colors.term], ["parcijalna suma Sₙ", colors.sum]]);
        const all = [...termValues, ...sumValues, ...(Number.isFinite(options.limit) ? [options.limit] : [])];
        plot(svg, termValues, { color: colors.term, scaleValues: all });
        const plotted = plot(svg, sumValues, { color: colors.sum, scaleValues: all });
        if (Number.isFinite(options.limit) && plotted) {
            const limitY = plotted.y(options.limit);
            svg.append(node("line", { x1: 58, y1: limitY, x2: 610, y2: limitY, stroke: colors.accent, "stroke-width": 2, "stroke-dasharray": "7 6" }));
            svg.append(node("text", { x: 604, y: limitY - 8, fill: colors.accent, "text-anchor": "end", "font-size": 12 }, `S∞ = ${options.limit.toLocaleString("hr-HR", { maximumFractionDigits: 4 })}`));
        }
        cache.set(containerId, () => combined(containerId, terms, sums, options));
    }
    function recurrence(containerId, terms, type) {
        const values = finite(terms), svg = shell(containerId, type === "fibonacci" ? "Fibonacci rekurzija" : "Rekurzivni niz");
        axes(svg); legend(svg, [[type === "fibonacci" ? "Fibonacci član" : "rekurzivni član", colors.accent]]);
        plot(svg, values, { color: colors.accent });
        if (type === "fibonacci" && values.length >= 3) {
            const shown = Math.min(values.length, 7);
            for (let index = 2; index < shown; index++) {
                const x = 75 + index * 75;
                svg.append(node("text", { x, y: 370, fill: colors.text, "text-anchor": "middle", "font-size": 12 }, `${values[index - 2]} + ${values[index - 1]} = ${values[index]}`));
            }
        }
        cache.set(containerId, () => recurrence(containerId, terms, type));
    }
    function application(containerId, values, type) {
        const data = finite(values), svg = shell(containerId, "Razvoj vrijednosti kroz razdoblja");
        axes(svg); const decreasing = type === "amortization" || data.at(-1) < data[0];
        legend(svg, [[type === "amortization" ? "preostala glavnica" : "vrijednost kroz vrijeme", decreasing ? colors.accent : colors.term]]);
        plot(svg, data, { color: decreasing ? colors.accent : colors.term });
        cache.set(containerId, () => application(containerId, values, type));
    }
    function refresh(panelId) {
        const visual = document.querySelector(`#${panelId} .sequences-visual`);
        cache.get(visual?.id)?.();
    }
    return { points, combined, recurrence, application, refresh };
})();
