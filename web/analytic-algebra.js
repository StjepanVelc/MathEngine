function setupAnalyticAlgebra(module) {
    const toggles = [...document.querySelectorAll(".analytic-algebra-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".analytic-algebra-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); } }));

    function number(id) { const input = document.getElementById(id); const value = Number(input.value); if (!input || input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj."); return value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "analytic-algebra-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }
    function matrixText(matrix) { return matrix.map((row) => `[${row.map(format).join(", ")}]`).join(", "); }

    function appendVectorDiagram(boxId, ax, ay, bx, by, cross) {
        const box = document.getElementById(boxId);
        const namespace = "http://www.w3.org/2000/svg";
        const figure = document.createElement("figure");
        figure.className = "algebra-plot";
        const figcaption = document.createElement("figcaption");
        figcaption.textContent = "Vektori a i b (projekcija na xy ravninu)";
        const svg = document.createElementNS(namespace, "svg");
        svg.setAttribute("viewBox", "0 0 640 320");
        svg.setAttribute("role", "img");
        svg.setAttribute("aria-label", figcaption.textContent);
        const maxMagnitude = Math.max(1, Math.abs(ax), Math.abs(ay), Math.abs(bx), Math.abs(by), Math.abs(cross || 0));
        const scale = 120 / maxMagnitude;
        const originX = 320, originY = 160;
        const toScreenX = (x) => originX + x * scale;
        const toScreenY = (y) => originY - y * scale;
        const defs = document.createElementNS(namespace, "defs");
        const makeMarker = (id, colorClass) => {
            const marker = document.createElementNS(namespace, "marker");
            marker.setAttribute("id", id);
            marker.setAttribute("markerWidth", "8");
            marker.setAttribute("markerHeight", "8");
            marker.setAttribute("refX", "4");
            marker.setAttribute("refY", "4");
            marker.setAttribute("orient", "auto");
            const path = document.createElementNS(namespace, "path");
            path.setAttribute("d", "M0,0 L8,4 L0,8 Z");
            path.setAttribute("class", colorClass);
            marker.append(path);
            defs.append(marker);
        };
        makeMarker("algebra-arrow-a", "algebra-plot-point");
        makeMarker("algebra-arrow-b", "algebra-plot-point");
        svg.append(defs);
        const addLine = (x1, y1, x2, y2, className) => {
            const line = document.createElementNS(namespace, "line");
            Object.entries({ x1, y1, x2, y2 }).forEach(([key, value]) => line.setAttribute(key, value));
            line.setAttribute("class", className);
            svg.append(line);
        };
        addLine(40, originY, 600, originY, "algebra-plot-axis");
        addLine(originX, 20, originX, 300, "algebra-plot-axis");
        addLine(originX, originY, toScreenX(ax), toScreenY(ay), "algebra-plot-vector-a");
        addLine(originX, originY, toScreenX(bx), toScreenY(by), "algebra-plot-vector-b");
        figure.append(figcaption, svg);
        box.append(figure);
    }

    // Vektori u prostoru
    function calculateSpaceVectors() {
        try {
            const ax = boundedNumber("aga-vectors-ax", { min: -1e6, max: 1e6 }), ay = boundedNumber("aga-vectors-ay", { min: -1e6, max: 1e6 }), az = boundedNumber("aga-vectors-az", { min: -1e6, max: 1e6 });
            const bx = boundedNumber("aga-vectors-bx", { min: -1e6, max: 1e6 }), by = boundedNumber("aga-vectors-by", { min: -1e6, max: 1e6 }), bz = boundedNumber("aga-vectors-bz", { min: -1e6, max: 1e6 });
            const result = parse(module.linearAlgebraSpaceVectors(ax, ay, az, bx, by, bz), "aga-vectors-result");
            if (!result) return;
            render("aga-vectors-result", [["Vektorski produkt", `(${format(result.cross.x)}, ${format(result.cross.y)}, ${format(result.cross.z)})`, true], ["Duljina produkta", format(result.crossMagnitude)], ["Skalarni produkt", format(result.dot)]]);
            steps("aga-vectors-steps", result.steps);
            const visualBox = document.getElementById("aga-vectors-visual");
            visualBox.innerHTML = "";
            if (az === 0 && bz === 0) appendVectorDiagram("aga-vectors-visual", ax, ay, bx, by, result.cross.z);
        } catch (error) { fail("aga-vectors-result", error); document.getElementById("aga-vectors-visual").innerHTML = ""; }
    }
    document.getElementById("aga-vectors-calculate").addEventListener("click", calculateSpaceVectors);

    function calculatePlaneFromPoints() {
        try {
            const x1 = boundedNumber("aga-plane-x1", { min: -1e6, max: 1e6 }), y1 = boundedNumber("aga-plane-y1", { min: -1e6, max: 1e6 }), z1 = boundedNumber("aga-plane-z1", { min: -1e6, max: 1e6 });
            const x2 = boundedNumber("aga-plane-x2", { min: -1e6, max: 1e6 }), y2 = boundedNumber("aga-plane-y2", { min: -1e6, max: 1e6 }), z2 = boundedNumber("aga-plane-z2", { min: -1e6, max: 1e6 });
            const x3 = boundedNumber("aga-plane-x3", { min: -1e6, max: 1e6 }), y3 = boundedNumber("aga-plane-y3", { min: -1e6, max: 1e6 }), z3 = boundedNumber("aga-plane-z3", { min: -1e6, max: 1e6 });
            const result = parse(module.linearAlgebraPlaneFromPoints(x1, y1, z1, x2, y2, z2, x3, y3, z3), "aga-plane-result");
            if (!result) return;
            render("aga-plane-result", [["Jednadžba ravnine", `${format(result.normalX)}x + ${format(result.normalY)}y + ${format(result.normalZ)}z + ${format(result.constant)} = 0`, true]]);
            steps("aga-plane-steps", result.steps);
        } catch (error) { fail("aga-plane-result", error); }
    }
    document.getElementById("aga-plane-calculate").addEventListener("click", calculatePlaneFromPoints);

    // Kvadrici
    function calculateQuadric() {
        try {
            const a = number("aga-quadric-a"), b = number("aga-quadric-b"), c = number("aga-quadric-c"), d = number("aga-quadric-d");
            const result = parse(module.linearAlgebraClassifyQuadricCentral(a, b, c, d), "aga-quadric-result");
            if (!result) return;
            render("aga-quadric-result", [["Vrsta kvadrike", result.typeName, true]]);
            steps("aga-quadric-steps", result.steps);
        } catch (error) { fail("aga-quadric-result", error); }
    }
    document.getElementById("aga-quadric-calculate").addEventListener("click", calculateQuadric);

    // Matrice
    function calculateMatrix() {
        try {
            const text = document.getElementById("aga-matrix-input").value.trim();
            if (!text) throw new Error("Upiši matricu.");
            const determinantResult = parse(module.linearAlgebraMatrixDeterminant(text), "aga-matrix-result");
            if (!determinantResult) return;
            const inverseResult = parse(module.linearAlgebraMatrixInverse(text), "aga-matrix-result");
            if (!inverseResult) return;
            const rows = [["Determinanta", format(determinantResult.determinant), true], ["Invertibilna", inverseResult.invertible ? "da" : "ne"]];
            if (inverseResult.invertible) rows.push(["Inverzna matrica", matrixText(inverseResult.inverse)]);
            render("aga-matrix-result", rows);
            steps("aga-matrix-steps", [...determinantResult.steps, ...inverseResult.steps]);
        } catch (error) { fail("aga-matrix-result", error); }
    }
    document.getElementById("aga-matrix-calculate").addEventListener("click", calculateMatrix);

    function calculateTransform() {
        try {
            const matrixInput = document.getElementById("aga-transform-matrix").value.trim();
            const vectorInput = document.getElementById("aga-transform-vector").value.trim();
            if (!matrixInput || !vectorInput) throw new Error("Upiši matricu i vektor.");
            const result = parse(module.linearAlgebraMatrixTransform(matrixInput, vectorInput), "aga-transform-result");
            if (!result) return;
            render("aga-transform-result", [["Transformirani vektor", `[${result.transformedVector.map(format).join(", ")}]`, true]]);
            steps("aga-transform-steps", result.steps);
        } catch (error) { fail("aga-transform-result", error); }
    }
    document.getElementById("aga-transform-calculate").addEventListener("click", calculateTransform);

    // Svojstvene vrijednosti
    function calculateEigen() {
        try {
            const text = document.getElementById("aga-eigen-input").value.trim();
            if (!text) throw new Error("Upiši simetricnu matricu.");
            const rows = text.split(";").map((row) => row.split(",").map(Number));
            const result = rows.length === 2
                ? parse(module.linearAlgebraEigenSymmetric2x2(rows[0][0], rows[0][1], rows[1][1]), "aga-eigen-result")
                : parse(module.linearAlgebraEigenSymmetric3x3(text), "aga-eigen-result");
            if (!result) return;
            const rowsOut = result.eigenPairs.map((pair, index) => [`λ${index + 1}`, `${format(pair.eigenvalue)}, vektor: [${pair.eigenvector.map(format).join(", ")}]`, index === 0]);
            render("aga-eigen-result", rowsOut);
            steps("aga-eigen-steps", result.steps);
        } catch (error) { fail("aga-eigen-result", error); }
    }
    document.getElementById("aga-eigen-calculate").addEventListener("click", calculateEigen);

    // Vektorski prostori
    function calculateVectorSpace() {
        try {
            const text = document.getElementById("aga-vectorspace-input").value.trim();
            if (!text) throw new Error("Upiši skup vektora.");
            const independence = parse(module.linearAlgebraLinearIndependence(text), "aga-vectorspace-result");
            if (!independence) return;
            const basis = parse(module.linearAlgebraExtractBasis(text), "aga-vectorspace-result");
            if (!basis) return;
            render("aga-vectorspace-result", [["Linearno nezavisni", independence.linearlyIndependent ? "da" : "ne", true], ["Rang", independence.rank], ["Baza", matrixText(basis.basisVectors)]]);
            steps("aga-vectorspace-steps", [...independence.steps, ...basis.steps]);
        } catch (error) { fail("aga-vectorspace-result", error); }
    }
    document.getElementById("aga-vectorspace-calculate").addEventListener("click", calculateVectorSpace);

    setupAnalyticAlgebraPractice({ root: document.getElementById("aga-practice-root"), format });
}
