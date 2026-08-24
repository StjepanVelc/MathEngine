window.TrigonometryVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg";
    const colors = { sine: "#f38ba8", cosine: "#89dceb", tangent: "#f9e2af", violet: "#cba6f7", text: "#cdd6f4", muted: "#6c7086", bg: "#181825" };
    const make = (id, viewBox = "0 0 420 320") => { const box = document.getElementById(id); if (!box) return null; box.innerHTML = ""; const svg = document.createElementNS(ns, "svg"); svg.setAttribute("viewBox", viewBox); svg.classList.add("trigonometry-svg"); box.append(svg); return svg; };
    const add = (svg, name, attrs = {}, text = "") => { const node = document.createElementNS(ns, name); Object.entries(attrs).forEach(([key, value]) => node.setAttribute(key, value)); if (text) node.textContent = text; svg.append(node); return node; };
    const line = (svg, x1, y1, x2, y2, color = colors.muted, width = 2, dash = "") => add(svg, "line", { x1, y1, x2, y2, stroke: color, "stroke-width": width, "stroke-dasharray": dash, "stroke-linecap": "round" });
    const text = (svg, x, y, value, color = colors.text, anchor = "middle", size = 14) => add(svg, "text", { x, y, fill: color, "text-anchor": anchor, "font-size": size, "font-weight": 650 }, value);
    const path = (svg, d, color, width = 3) => add(svg, "path", { d, fill: "none", stroke: color, "stroke-width": width, "stroke-linejoin": "round", "stroke-linecap": "round", class: "trigonometry-draw" });
    const f = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 3 }).format(value);

    function circle(result = { degrees: 30, x: Math.sqrt(3) / 2, y: 0.5, quadrant: 1, referenceDegrees: 30 }) {
        const svg = make("trigonometry-circle-visual"); if (!svg) return;
        const cx = 205, cy = 160, r = 115, px = cx + result.x * r, py = cy - result.y * r;
        line(svg, 45, cy, 365, cy, colors.muted); line(svg, cx, 25, cx, 295, colors.muted);
        add(svg, "circle", { cx, cy, r, fill: "#89dceb0d", stroke: colors.cosine, "stroke-width": 3 });
        [0, 90, 180, 270].forEach((angle) => { const rad = angle * Math.PI / 180; text(svg, cx + Math.cos(rad) * (r + 18), cy - Math.sin(rad) * (r + 18) + 5, `${angle}°`, colors.muted, "middle", 11); });
        line(svg, cx, cy, px, py, colors.violet, 4); line(svg, px, py, px, cy, colors.sine, 3, "6 5"); line(svg, cx, cy, px, cy, colors.cosine, 5);
        add(svg, "circle", { cx: px, cy: py, r: 7, fill: colors.tangent, stroke: colors.bg, "stroke-width": 3, class: "trigonometry-pop" });
        const end = (result.degrees % 360 + 360) % 360; const sweep = end <= 180 ? 0 : 1; const ar = 36; const ex = cx + Math.cos(end * Math.PI / 180) * ar, ey = cy - Math.sin(end * Math.PI / 180) * ar;
        path(svg, `M ${cx + ar} ${cy} A ${ar} ${ar} 0 ${sweep} 0 ${ex} ${ey}`, colors.tangent, 3);
        text(svg, cx + 47, cy - 15, `α=${f(result.degrees)}°`, colors.tangent, "start");
        text(svg, (cx + px) / 2, cy + 22, `cos α = ${f(result.x)}`, colors.cosine);
        text(svg, px + 10, (cy + py) / 2, `sin α = ${f(result.y)}`, colors.sine, "start");
        text(svg, 205, 308, result.quadrant ? `${result.quadrant}. kvadrant · referentni kut ${f(result.referenceDegrees)}°` : "Kut završava na koordinatnoj osi", colors.text);
    }

    function functionValue(name, radians) { return name === "sin" ? Math.sin(radians) : name === "cos" ? Math.cos(radians) : Math.tan(radians); }
    function wave(name = "sin", angleDegrees = 30, value) {
        const svg = make("trigonometry-wave-visual", "0 0 500 320"); if (!svg) return;
        const color = name === "sin" ? colors.sine : name === "cos" ? colors.cosine : colors.tangent;
        const left = 45, right = 475, mid = 160, sx = (right - left) / 720, sy = name === "tan" ? 45 : 100;
        line(svg, left, mid, right, mid); line(svg, left, 25, left, 295);
        [-360, -180, 0, 180, 360].forEach((degree) => { const x = left + (degree + 360) * sx; line(svg, x, mid - 5, x, mid + 5); text(svg, x, mid + 22, `${degree}°`, colors.muted, "middle", 10); });
        let d = "", drawing = false;
        for (let degree = -360; degree <= 360; degree += 2) {
            const yValue = functionValue(name, degree * Math.PI / 180);
            const valid = Number.isFinite(yValue) && Math.abs(yValue) <= 3;
            if (!valid) { drawing = false; continue; }
            const x = left + (degree + 360) * sx, y = mid - yValue * sy;
            d += `${drawing ? " L" : " M"} ${x} ${y}`; drawing = true;
        }
        path(svg, d, color, 3);
        const normalized = Math.max(-360, Math.min(360, angleDegrees)); const shown = Number.isFinite(value) ? value : functionValue(name, normalized * Math.PI / 180);
        if (Number.isFinite(shown) && Math.abs(shown) <= 3) {
            const x = left + (normalized + 360) * sx, y = mid - shown * sy;
            line(svg, x, mid, x, y, colors.tangent, 2, "5 5"); add(svg, "circle", { cx: x, cy: y, r: 6, fill: color, class: "trigonometry-pop" });
            text(svg, x, y - 13, `${name}(${f(angleDegrees)}°)=${f(shown)}`, color);
        }
        text(svg, 250, 30, name === "tan" ? "Tangens se ponavlja svakih 180°" : `${name} se ponavlja svakih 360°`, color, "middle", 16);
    }

    function rightTriangle(result = { opposite: 3, adjacent: 4, hypotenuse: 5, angleDegrees: 36.87 }) {
        const svg = make("trigonometry-right-visual"); if (!svg) return;
        const a = Number(result.opposite) || 3, b = Number(result.adjacent) || 4; const scale = Math.min(210 / b, 180 / a); const x = 75, y = 260, bx = x + b * scale, ty = y - a * scale;
        add(svg, "polygon", { points: `${x},${y} ${bx},${y} ${x},${ty}`, fill: "#cba6f722", stroke: colors.violet, "stroke-width": 4, "stroke-linejoin": "round", class: "trigonometry-draw" });
        add(svg, "polyline", { points: `${x},${y - 20} ${x + 20},${y - 20} ${x + 20},${y}`, fill: "none", stroke: colors.tangent, "stroke-width": 3 });
        text(svg, (x + bx) / 2, y + 25, `priležeća = ${f(result.adjacent)}`, colors.cosine);
        text(svg, x - 12, (y + ty) / 2, `nasuprotna = ${f(result.opposite)}`, colors.sine, "end");
        text(svg, (x + bx) / 2 + 35, (y + ty) / 2 - 8, `c = ${f(result.hypotenuse)}`, colors.violet);
        text(svg, bx - 15, y - 12, `α=${f(result.angleDegrees)}°`, colors.tangent, "end");
        text(svg, 210, 38, "sin α = nasuprotna / hipotenuza", colors.sine, "middle", 15);
        text(svg, 210, 60, "cos α = priležeća / hipotenuza", colors.cosine, "middle", 15);
    }

    function generalTriangle(result = { sideA: 5, sideB: 7, sideC: 6, angleA: 45, angleB: 75, angleC: 60 }) {
        const svg = make("trigonometry-general-visual"); if (!svg) return;
        const A = [65, 255], B = [355, 255], C = [205, 55];
        add(svg, "polygon", { points: `${A} ${B} ${C}`, fill: "#89dceb18", stroke: colors.cosine, "stroke-width": 4, "stroke-linejoin": "round", class: "trigonometry-draw" });
        text(svg, 210, 282, `c = ${f(result.sideC)}`, colors.tangent); text(svg, 118, 150, `b = ${f(result.sideB)}`, colors.sine); text(svg, 302, 150, `a = ${f(result.sideA)}`, colors.cosine);
        text(svg, A[0] + 25, A[1] - 15, `α=${f(result.angleA)}°`, colors.violet, "start"); text(svg, B[0] - 25, B[1] - 15, `β=${f(result.angleB)}°`, colors.violet, "end"); text(svg, C[0], C[1] + 26, `γ=${f(result.angleC)}°`, colors.violet);
        text(svg, 210, 25, "a / sin α = b / sin β = c / sin γ", colors.text, "middle", 15);
    }

    function identity(result = { formula: "sin^2(x) + cos^2(x) = 1", left: 1, right: 1 }) {
        const svg = make("trigonometry-identity-visual"); if (!svg) return;
        add(svg, "circle", { cx: 210, cy: 155, r: 105, fill: "#89dceb0d", stroke: colors.cosine, "stroke-width": 3 });
        const x = 210 + 78, y = 155 - 70; line(svg, 210, 155, x, y, colors.violet, 4); line(svg, x, y, x, 155, colors.sine, 4); line(svg, 210, 155, x, 155, colors.cosine, 4);
        text(svg, 210, 28, result.formula, colors.tangent, "middle", 17); text(svg, 249, 177, "cos x", colors.cosine); text(svg, 300, 120, "sin x", colors.sine, "start");
        text(svg, 210, 295, `L = ${f(result.left)}  ·  D = ${f(result.right)}`, result.holds ? "#a6e3a1" : colors.sine, "middle", 17);
    }

    function equation(name = "sin", target = 0.5, solutions = []) {
        waveInto("trigonometry-identity-visual", name, target, solutions);
    }
    function waveInto(id, name, target, solutions) {
        const svg = make(id, "0 0 500 320"); if (!svg) return; const color = name === "sin" ? colors.sine : name === "cos" ? colors.cosine : colors.tangent; const left = 45, right = 475, mid = 160, sx = (right - left) / 360, sy = name === "tan" ? 45 : 100;
        line(svg, left, mid, right, mid); line(svg, left, 25, left, 295); const targetY = mid - target * sy; line(svg, left, targetY, right, targetY, colors.tangent, 2, "7 5"); text(svg, right - 5, targetY - 8, `y=${f(target)}`, colors.tangent, "end");
        let d = "", drawing = false; for (let degree = 0; degree <= 360; degree += 2) { const val = functionValue(name, degree * Math.PI / 180); if (!Number.isFinite(val) || Math.abs(val) > 3) { drawing = false; continue; } const x = left + degree * sx, y = mid - val * sy; d += `${drawing ? " L" : " M"} ${x} ${y}`; drawing = true; } path(svg, d, color, 3);
        solutions.filter((x) => x >= 0 && x <= 360).forEach((solution) => { const x = left + solution * sx; add(svg, "circle", { cx: x, cy: targetY, r: 6, fill: colors.tangent, class: "trigonometry-pop" }); text(svg, x, targetY + 22, `${f(solution)}°`, colors.text); });
    }
    function refresh(panelId) { if (panelId === "trigonometry-circle") document.getElementById("trigonometry-angle-analyze")?.click(); }
    return { circle, wave, rightTriangle, generalTriangle, identity, equation, refresh };
})();
