window.GeometryVisuals = (() => {
    const ns = "http://www.w3.org/2000/svg";
    const colors = { blue: "#74c7ec", aqua: "#89dceb", violet: "#cba6f7", yellow: "#f9e2af", text: "#cdd6f4", muted: "#7f849c", bg: "#181825" };

    function svgFor(containerId, viewBox = "0 0 360 280") {
        const container = document.getElementById(containerId);
        if (!container) return null;
        container.innerHTML = "";
        const svg = document.createElementNS(ns, "svg");
        svg.setAttribute("viewBox", viewBox);
        svg.setAttribute("role", "img");
        svg.classList.add("geometry-svg");
        container.append(svg);
        return svg;
    }

    function element(svg, name, attributes = {}, text = "") {
        const node = document.createElementNS(ns, name);
        Object.entries(attributes).forEach(([key, value]) => node.setAttribute(key, value));
        if (text) node.textContent = text;
        svg.append(node);
        return node;
    }

    function line(svg, x1, y1, x2, y2, options = {}) {
        return element(svg, "line", { x1, y1, x2, y2, stroke: options.stroke || colors.muted, "stroke-width": options.width || 2, "stroke-dasharray": options.dash || "", "stroke-linecap": "round" });
    }

    function label(svg, x, y, text, options = {}) {
        return element(svg, "text", { x, y, fill: options.fill || colors.text, "font-size": options.size || 15, "font-weight": options.weight || 650, "text-anchor": options.anchor || "middle" }, text);
    }

    function polygon(svg, points, fill = "#74c7ec33") {
        return element(svg, "polygon", { points, fill, stroke: colors.blue, "stroke-width": 4, "stroke-linejoin": "round", class: "geometry-draw" });
    }

    function positive(value, fallback = 1) {
        return Number.isFinite(value) && value > 0 ? value : fallback;
    }

    function units(category, from, to) {
        const svg = svgFor("geometry-unit-visual");
        if (!svg) return;
        const titles = { length: "Duljina", area: "Površina", volume: "Obujam" };
        label(svg, 180, 38, titles[category], { size: 22, fill: colors.aqua, weight: 800 });
        const count = category === "length" ? 4 : 3;
        for (let index = 0; index < count; index++) {
            const width = 220 - index * 42;
            const height = category === "length" ? 18 : 30 + index * 7;
            element(svg, "rect", { x: 70 + index * 21, y: 75 + index * 43, width, height, rx: 9, fill: index % 2 ? "#cba6f744" : "#89dceb44", stroke: index % 2 ? colors.violet : colors.aqua, "stroke-width": 2, class: "geometry-pop" });
        }
        line(svg, 75, 235, 285, 235, { stroke: colors.yellow, width: 4 });
        element(svg, "path", { d: "M275 226 L290 235 L275 244", fill: "none", stroke: colors.yellow, "stroke-width": 4, "stroke-linecap": "round", "stroke-linejoin": "round" });
        label(svg, 95, 263, from, { fill: colors.yellow, size: 18 });
        label(svg, 265, 263, to, { fill: colors.yellow, size: 18 });
        label(svg, 180, 263, "pretvori", { fill: colors.muted, size: 12 });
    }

    function shape(kind, values = {}) {
        const svg = svgFor("geometry-shape-visual");
        if (!svg) return;
        const a = positive(values.a, 5), b = positive(values.b, 3), c = positive(values.c, 4), d = positive(values.d, 4), h = positive(values.h, 3);
        label(svg, 180, 30, ({ square: "Kvadrat", rectangle: "Pravokutnik", triangle: "Trokut", parallelogram: "Paralelogram", trapezoid: "Trapez", circle: "Krug" })[kind], { size: 21, fill: colors.aqua, weight: 800 });
        if (kind === "circle") {
            const radius = 82;
            element(svg, "circle", { cx: 180, cy: 145, r: radius, fill: "#cba6f733", stroke: colors.violet, "stroke-width": 4, class: "geometry-draw" });
            line(svg, 180, 145, 262, 145, { stroke: colors.yellow, width: 3 });
            element(svg, "circle", { cx: 180, cy: 145, r: 4, fill: colors.yellow });
            label(svg, 220, 136, `r = ${a}`, { fill: colors.yellow });
            return;
        }
        if (kind === "triangle") {
            polygon(svg, "70,225 290,225 205,62");
            label(svg, 180, 248, `a = ${a}`);
            label(svg, 124, 140, `b = ${b}`);
            label(svg, 256, 140, `c = ${c}`);
            return;
        }
        let points;
        if (kind === "parallelogram") points = "90,215 270,215 225,75 45,75";
        else if (kind === "trapezoid") points = "55,215 305,215 250,75 110,75";
        else {
            const ratio = kind === "square" ? 1 : Math.min(2.1, Math.max(0.55, a / b));
            const width = ratio >= 1 ? 210 : 150;
            const height = ratio >= 1 ? 210 / ratio : 150 / ratio;
            const x = 180 - width / 2, y = 145 - height / 2;
            points = `${x},${y} ${x + width},${y} ${x + width},${y + height} ${x},${y + height}`;
        }
        polygon(svg, points);
        label(svg, 180, 252, `a = ${a}`);
        if (kind !== "square") label(svg, 322, 145, `b = ${b}`, { anchor: "end" });
        if (kind === "parallelogram" || kind === "trapezoid") {
            line(svg, 225, 75, 225, 215, { stroke: colors.yellow, width: 2, dash: "6 6" });
            label(svg, 245, 150, `h = ${h}`, { fill: colors.yellow, anchor: "start" });
        }
        if (kind === "trapezoid") label(svg, 180, 66, `b = ${b}`);
        void c; void d;
    }

    function triangle(operation, values = {}) {
        const svg = svgFor("geometry-triangle-visual");
        if (!svg) return;
        const a = positive(values.a, 3), b = positive(values.b, 4), c = positive(values.c, 5);
        const right = operation === "hypotenuse" || operation === "leg" || (operation === "classify" && Math.abs(a * a + b * b - c * c) < 0.001);
        const points = right ? "65,225 295,225 65,65" : "55,225 305,225 190,60";
        polygon(svg, points, "#89dceb2e");
        if (right) {
            element(svg, "polyline", { points: "65,205 85,205 85,225", fill: "none", stroke: colors.yellow, "stroke-width": 3 });
            label(svg, 180, 250, operation === "leg" ? `poznata = ${b}` : `a = ${a}`);
            label(svg, 48, 150, operation === "leg" ? "?" : `b = ${b}`, { fill: colors.aqua });
            label(svg, 205, 132, operation === "leg" ? `c = ${a}` : "c = ?", { fill: colors.yellow });
        } else if (operation === "thirdAngle") {
            label(svg, 74, 214, `α=${a}°`, { fill: colors.yellow, anchor: "start" });
            label(svg, 286, 214, `β=${b}°`, { fill: colors.yellow, anchor: "end" });
            label(svg, 190, 88, "γ = ?", { fill: colors.violet });
        } else {
            label(svg, 180, 250, `a = ${a}`);
            label(svg, 100, 145, `b = ${b}`);
            label(svg, 267, 145, `c = ${c}`);
        }
    }

    function solid(kind, values = {}) {
        const svg = svgFor("geometry-solid-visual");
        if (!svg) return;
        const a = positive(values.a, 3), b = positive(values.b, 3), c = positive(values.c, 4);
        label(svg, 180, 30, ({ cube: "Kocka", cuboid: "Kvadar", prism: "Prizma", cylinder: "Valjak" })[kind], { size: 21, fill: colors.aqua, weight: 800 });
        if (kind === "cylinder") {
            element(svg, "path", { d: "M85 82 C85 55 275 55 275 82 L275 213 C275 240 85 240 85 213 Z", fill: "#74c7ec28", stroke: colors.blue, "stroke-width": 4, class: "geometry-draw" });
            element(svg, "ellipse", { cx: 180, cy: 82, rx: 95, ry: 25, fill: "#89dceb44", stroke: colors.aqua, "stroke-width": 4 });
            element(svg, "path", { d: "M85 213 C85 186 275 186 275 213", fill: "none", stroke: colors.blue, "stroke-width": 3, "stroke-dasharray": "7 6" });
            line(svg, 180, 82, 270, 82, { stroke: colors.yellow, width: 3 });
            label(svg, 225, 70, `r=${a}`, { fill: colors.yellow });
            label(svg, 298, 150, `h=${b}`, { anchor: "end" });
            return;
        }
        const front = kind === "prism" ? "80,210 175,210 128,105" : "75,205 235,205 235,85 75,85";
        const back = kind === "prism" ? "145,165 240,165 193,60" : "125,160 285,160 285,40 125,40";
        polygon(svg, back, "#cba6f722");
        polygon(svg, front, "#74c7ec2e");
        const frontPoints = kind === "prism" ? [[80,210],[175,210],[128,105]] : [[75,205],[235,205],[235,85],[75,85]];
        const backPoints = kind === "prism" ? [[145,165],[240,165],[193,60]] : [[125,160],[285,160],[285,40],[125,40]];
        frontPoints.forEach((point, index) => line(svg, point[0], point[1], backPoints[index][0], backPoints[index][1], { stroke: colors.violet, width: 3 }));
        label(svg, 150, 238, kind === "prism" ? `B=${a}` : `a=${a}`);
        label(svg, 300, 126, kind === "prism" ? `h=${c}` : kind === "cube" ? `a=${a}` : `c=${c}`, { anchor: "end" });
        if (kind === "cuboid") label(svg, 255, 190, `b=${b}`, { fill: colors.yellow });
    }

    function coordinates(x1, y1, x2, y2) {
        const svg = svgFor("geometry-coordinate-visual", "0 0 360 320");
        if (!svg) return;
        const values = [x1, y1, x2, y2].map((value) => Number.isFinite(value) ? value : 0);
        const bound = Math.max(5, ...values.map(Math.abs)) + 1;
        const scale = 125 / bound;
        const mapX = (value) => 180 + value * scale;
        const mapY = (value) => 160 - value * scale;
        for (let value = -Math.floor(bound); value <= Math.floor(bound); value++) {
            line(svg, mapX(value), 25, mapX(value), 295, { stroke: value === 0 ? colors.text : "#45475a", width: value === 0 ? 2.5 : 1 });
            line(svg, 25, mapY(value), 335, mapY(value), { stroke: value === 0 ? colors.text : "#45475a", width: value === 0 ? 2.5 : 1 });
        }
        line(svg, mapX(x1), mapY(y1), mapX(x2), mapY(y2), { stroke: colors.yellow, width: 3, dash: "7 6" });
        const middleX = (x1 + x2) / 2, middleY = (y1 + y2) / 2;
        [[x1, y1, "A", colors.aqua], [x2, y2, "B", colors.violet], [middleX, middleY, "S", colors.yellow]].forEach(([x, y, name, color]) => {
            element(svg, "circle", { cx: mapX(x), cy: mapY(y), r: name === "S" ? 5 : 7, fill: color, stroke: colors.bg, "stroke-width": 2, class: "geometry-pop" });
            label(svg, mapX(x) + 12, mapY(y) - 10, `${name}(${x}, ${y})`, { fill: color, anchor: "start", size: 12 });
        });
    }

    function refresh(panelId) {
        if (panelId === "geometry-coordinate-visual" || panelId === "geometry-coordinates") {
            const get = (id) => Number(document.getElementById(id)?.value || 0);
            coordinates(get("geometry-x1"), get("geometry-y1"), get("geometry-x2"), get("geometry-y2"));
        }
    }

    return { refresh, units, shape, triangle, solid, coordinates };
})();
