window.MathematicalAnalysisVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg", cache = new Map();
    const colors = { line: "#89dceb", point: "#a6e3a1", target: "#f38ba8", grid: "#45475a", text: "#cdd6f4", secondary: "#fab387" };
    function node(name, attrs = {}, text = "") { const element = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => element.setAttribute(key, value)); if (text) element.textContent = text; return element; }

    function taylorApproximation(containerId, center, taylorFn, actualFn, point) {
        const container = document.getElementById(containerId); if (!container) return; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 420", class: "mathematical-analysis-svg", role: "img", "aria-label": "Taylorova aproksimacija" }); svg.append(node("title", {}, "Taylorova aproksimacija")); container.append(svg);
        const plotLeft = 50, plotRight = 610, plotTop = 30, plotBottom = 380;
        const minX = center - 3, maxX = center + 3;
        const samples = []; for (let i = 0; i <= 60; i++) { const x = minX + (maxX - minX) * i / 60; samples.push({ x, actual: actualFn(x), taylor: taylorFn(x) }); }
        const values = samples.flatMap((s) => [s.actual, s.taylor]).filter(Number.isFinite);
        const minY = Math.min(...values), maxY = Math.max(...values);
        const spanX = maxX - minX || 1, spanY = (maxY - minY) || 1;
        const toX = (x) => plotLeft + ((x - minX) / spanX) * (plotRight - plotLeft);
        const toY = (y) => plotBottom - ((y - minY) / spanY) * (plotBottom - plotTop);
        svg.append(node("line", { x1: plotLeft, y1: plotBottom, x2: plotRight, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: plotLeft, y1: plotTop, x2: plotLeft, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        const actualPath = samples.filter((s) => Number.isFinite(s.actual)).map((s, i) => `${i === 0 ? "M" : "L"}${toX(s.x)},${toY(s.actual)}`).join(" ");
        const taylorPath = samples.filter((s) => Number.isFinite(s.taylor)).map((s, i) => `${i === 0 ? "M" : "L"}${toX(s.x)},${toY(s.taylor)}`).join(" ");
        svg.append(node("path", { d: actualPath, fill: "none", stroke: colors.line, "stroke-width": 3 }));
        svg.append(node("path", { d: taylorPath, fill: "none", stroke: colors.secondary, "stroke-width": 3, "stroke-dasharray": "6 4" }));
        svg.append(node("circle", { cx: toX(point), cy: toY(actualFn(point)), r: 6, fill: colors.target }));
        cache.set(containerId, () => taylorApproximation(containerId, center, taylorFn, actualFn, point));
    }

    function odeTrajectory(containerId, pointsEuler, pointsRk4) {
        const container = document.getElementById(containerId); if (!container) return; container.innerHTML = "";
        const svg = node("svg", { viewBox: "0 0 640 420", class: "mathematical-analysis-svg", role: "img", "aria-label": "Rjesenje diferencijalne jednadzbe" }); svg.append(node("title", {}, "Numericko rjesenje ODE")); container.append(svg);
        const plotLeft = 50, plotRight = 610, plotTop = 30, plotBottom = 380;
        const all = [...pointsEuler, ...pointsRk4];
        const minX = Math.min(...all.map((p) => p.t)), maxX = Math.max(...all.map((p) => p.t));
        const minY = Math.min(...all.map((p) => p.y)), maxY = Math.max(...all.map((p) => p.y));
        const spanX = (maxX - minX) || 1, spanY = (maxY - minY) || 1;
        const toX = (x) => plotLeft + ((x - minX) / spanX) * (plotRight - plotLeft);
        const toY = (y) => plotBottom - ((y - minY) / spanY) * (plotBottom - plotTop);
        svg.append(node("line", { x1: plotLeft, y1: plotBottom, x2: plotRight, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        svg.append(node("line", { x1: plotLeft, y1: plotTop, x2: plotLeft, y2: plotBottom, stroke: colors.text, "stroke-width": 2 }));
        const eulerPath = pointsEuler.map((p, i) => `${i === 0 ? "M" : "L"}${toX(p.t)},${toY(p.y)}`).join(" ");
        const rk4Path = pointsRk4.map((p, i) => `${i === 0 ? "M" : "L"}${toX(p.t)},${toY(p.y)}`).join(" ");
        svg.append(node("path", { d: eulerPath, fill: "none", stroke: colors.secondary, "stroke-width": 3 }));
        svg.append(node("path", { d: rk4Path, fill: "none", stroke: colors.line, "stroke-width": 3, "stroke-dasharray": "6 4" }));
        cache.set(containerId, () => odeTrajectory(containerId, pointsEuler, pointsRk4));
    }

    function refresh(panelId) { const visual = document.querySelector(`#${panelId} .mathematical-analysis-visual`); cache.get(visual?.id)?.(); }
    return { taylorApproximation, odeTrajectory, refresh };
})();
