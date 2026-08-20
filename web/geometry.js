function setupGeometry(module) {
    const visuals = window.GeometryVisuals || {};
    const toggles = [...document.querySelectorAll(".geometry-toggle")];

    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".geometry-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) {
            panel.hidden = false;
            button.classList.add("active");
            visuals.refresh?.(panel.id);
        }
    }));

    function number(id) {
        const input = document.getElementById(id);
        if (!input || input.value.trim() === "") throw new Error("Popuni sva potrebna polja.");
        const value = Number(input.value);
        if (!Number.isFinite(value)) throw new Error("Upiši ispravne brojeve.");
        return value;
    }

    function render(boxId, rows) {
        const box = document.getElementById(boxId);
        box.innerHTML = "";
        box.classList.remove("error");
        rows.forEach(([label, value, primary = false]) => {
            const row = document.createElement("div");
            row.className = `result-row${primary ? " result-primary" : ""}`;
            const name = document.createElement("span");
            name.textContent = label;
            const content = document.createElement("strong");
            content.textContent = value;
            row.append(name, content);
            box.append(row);
        });
    }

    function fail(boxId, message) {
        const box = document.getElementById(boxId);
        box.innerHTML = "";
        box.textContent = message.startsWith("GRESKA:") ? message : `GRESKA: ${message}`;
        box.classList.add("error");
    }

    function parse(text, boxId) {
        if (text.startsWith("GRESKA:")) {
            fail(boxId, text);
            return null;
        }
        return JSON.parse(text);
    }

    function format(value) {
        return new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    }

    const units = {
        length: [["mm", 0], ["cm", 1], ["dm", 2], ["m", 3], ["km", 4]],
        area: [["mm²", 0], ["cm²", 1], ["dm²", 2], ["m²", 3], ["ha", 4], ["km²", 5]],
        volume: [["mm³", 0], ["cm³", 1], ["dm³", 2], ["m³", 3], ["ml", 4], ["l", 5]]
    };
    const unitCategory = document.getElementById("geometry-unit-category");
    const unitFrom = document.getElementById("geometry-unit-from");
    const unitTo = document.getElementById("geometry-unit-to");

    function updateUnits() {
        const options = units[unitCategory.value];
        [unitFrom, unitTo].forEach((select, index) => {
            select.innerHTML = "";
            options.forEach(([label, value]) => select.add(new Option(label, value)));
            select.selectedIndex = index ? Math.min(1, options.length - 1) : 0;
        });
        visuals.units?.(unitCategory.value, unitFrom.options[unitFrom.selectedIndex].text, unitTo.options[unitTo.selectedIndex].text);
    }
    unitCategory.addEventListener("change", updateUnits);
    unitFrom.addEventListener("change", () => visuals.units?.(unitCategory.value, unitFrom.selectedOptions[0].text, unitTo.selectedOptions[0].text));
    unitTo.addEventListener("change", () => visuals.units?.(unitCategory.value, unitFrom.selectedOptions[0].text, unitTo.selectedOptions[0].text));
    document.getElementById("geometry-unit-calculate").addEventListener("click", () => {
        try {
            const result = parse(module.geometryConvert(unitCategory.value, number("geometry-unit-value"), Number(unitFrom.value), Number(unitTo.value)), "geometry-unit-result");
            if (result) render("geometry-unit-result", [["Pretvorena vrijednost", `${format(result.value)} ${unitTo.selectedOptions[0].text}`, true], ["Polazna mjera", `${format(number("geometry-unit-value"))} ${unitFrom.selectedOptions[0].text}`]]);
        } catch (error) { fail("geometry-unit-result", error.message); }
    });
    updateUnits();

    const shapeDefinitions = {
        square: { fields: [["a", "Stranica a", 5]], formula: ["Kvadrat", "O = 4a", "P = a²"] },
        rectangle: { fields: [["a", "Duljina a", 7], ["b", "Širina b", 3]], formula: ["Pravokutnik", "O = 2(a + b)", "P = a · b"] },
        triangle: { fields: [["a", "Stranica a", 3], ["b", "Stranica b", 4], ["c", "Stranica c", 5]], formula: ["Trokut", "O = a + b + c", "P = √(s(s−a)(s−b)(s−c))"] },
        parallelogram: { fields: [["a", "Osnovica a", 8], ["b", "Stranica b", 5], ["h", "Visina h", 4]], formula: ["Paralelogram", "O = 2(a + b)", "P = a · h"] },
        trapezoid: { fields: [["a", "Osnovica a", 10], ["b", "Osnovica b", 6], ["c", "Stranica c", 5], ["d", "Stranica d", 5], ["h", "Visina h", 4]], formula: ["Trapez", "O = a + b + c + d", "P = (a + b) · h / 2"] },
        circle: { fields: [["a", "Polumjer r", 3]], formula: ["Krug", "O = 2πr", "P = πr²"] }
    };
    const shapeSelect = document.getElementById("geometry-shape");
    function createFields(containerId, fields, prefix) {
        const container = document.getElementById(containerId);
        container.innerHTML = "";
        fields.forEach(([key, label, value]) => {
            const wrapper = document.createElement("label");
            wrapper.textContent = label;
            const input = document.createElement("input");
            input.id = `${prefix}-${key}`;
            input.type = "number";
            input.step = "any";
            input.value = value;
            input.addEventListener("input", () => visuals.refresh?.(container.closest(".geometry-panel").id));
            wrapper.append(input);
            container.append(wrapper);
        });
    }
    function formula(id, definition) {
        const card = document.getElementById(id);
        card.innerHTML = `<strong>${definition[0]}</strong><code>${definition[1]}</code><code>${definition[2]}</code>`;
    }
    function updateShape() {
        const definition = shapeDefinitions[shapeSelect.value];
        createFields("geometry-shape-inputs", definition.fields, "geometry-shape");
        formula("geometry-shape-formula", definition.formula);
        visuals.shape?.(shapeSelect.value, shapeValues(false));
    }
    function shapeValues(required = true) {
        const values = {};
        shapeDefinitions[shapeSelect.value].fields.forEach(([key]) => {
            const input = document.getElementById(`geometry-shape-${key}`);
            values[key] = required ? number(input.id) : Number(input?.value || 0);
        });
        return values;
    }
    shapeSelect.addEventListener("change", updateShape);
    document.getElementById("geometry-shape-inputs").addEventListener("input", () => visuals.shape?.(shapeSelect.value, shapeValues(false)));
    document.getElementById("geometry-shape-calculate").addEventListener("click", () => {
        try {
            const value = shapeValues();
            const result = parse(module.geometryPlaneShape(shapeSelect.value, value.a || 0, value.b || 0, value.c || 0, value.d || 0, value.h || 0), "geometry-shape-result");
            if (result) render("geometry-shape-result", [["Opseg", `${format(result.perimeter)} jedinica`, true], ["Površina", `${format(result.area)} kvadratnih jedinica`]]);
        } catch (error) { fail("geometry-shape-result", error.message); }
    });
    updateShape();

    const triangleDefinitions = {
        classify: { fields: [["a", "Stranica a", 3], ["b", "Stranica b", 4], ["c", "Stranica c", 5]], formula: ["Usporedi stranice i kutove", "a² + b² = c²", "Pravokutan trokut"] },
        thirdAngle: { fields: [["a", "Kut α", 45], ["b", "Kut β", 65]], formula: ["Zbroj kutova trokuta", "α + β + γ = 180°", "γ = 180° − α − β"] },
        hypotenuse: { fields: [["a", "Kateta a", 3], ["b", "Kateta b", 4]], formula: ["Pitagorin poučak", "a² + b² = c²", "c = √(a² + b²)"] },
        leg: { fields: [["a", "Hipotenuza c", 13], ["b", "Poznata kateta", 5]], formula: ["Pitagorin poučak", "a² + b² = c²", "a = √(c² − b²)"] }
    };
    const triangleOperation = document.getElementById("geometry-triangle-operation");
    function triangleValues(required = true) {
        const values = {};
        triangleDefinitions[triangleOperation.value].fields.forEach(([key]) => {
            const input = document.getElementById(`geometry-triangle-${key}`);
            values[key] = required ? number(input.id) : Number(input?.value || 0);
        });
        return values;
    }
    function updateTriangle() {
        const definition = triangleDefinitions[triangleOperation.value];
        createFields("geometry-triangle-inputs", definition.fields, "geometry-triangle");
        formula("geometry-triangle-formula", definition.formula);
        visuals.triangle?.(triangleOperation.value, triangleValues(false));
    }
    triangleOperation.addEventListener("change", updateTriangle);
    document.getElementById("geometry-triangle-inputs").addEventListener("input", () => visuals.triangle?.(triangleOperation.value, triangleValues(false)));
    document.getElementById("geometry-triangle-calculate").addEventListener("click", () => {
        try {
            const value = triangleValues();
            const result = parse(module.geometryTriangle(triangleOperation.value, value.a || 0, value.b || 0, value.c || 0), "geometry-triangle-result");
            if (!result) return;
            if (triangleOperation.value === "classify") render("geometry-triangle-result", [["Prema stranicama", result.bySides, true], ["Prema kutovima", result.byAngles]]);
            else render("geometry-triangle-result", [[triangleOperation.value === "thirdAngle" ? "Treći kut" : "Nepoznata stranica", `${format(result.value)}${triangleOperation.value === "thirdAngle" ? "°" : " jedinica"}`, true]]);
        } catch (error) { fail("geometry-triangle-result", error.message); }
    });
    updateTriangle();

    const solidDefinitions = {
        cube: { fields: [["a", "Brid a", 3]], formula: ["Kocka", "O = 6a²", "V = a³"] },
        cuboid: { fields: [["a", "Duljina a", 2], ["b", "Širina b", 3], ["c", "Visina c", 4]], formula: ["Kvadar", "O = 2(ab + ac + bc)", "V = abc"] },
        prism: { fields: [["a", "Površina baze B", 6], ["b", "Opseg baze Oᵦ", 12], ["c", "Visina h", 10]], formula: ["Uspravna prizma", "O = 2B + Oᵦh", "V = Bh"] },
        cylinder: { fields: [["a", "Polumjer r", 2], ["b", "Visina h", 5]], formula: ["Valjak", "O = 2πr(r + h)", "V = πr²h"] }
    };
    const solidSelect = document.getElementById("geometry-solid");
    function solidValues(required = true) {
        const values = {};
        solidDefinitions[solidSelect.value].fields.forEach(([key]) => {
            const input = document.getElementById(`geometry-solid-${key}`);
            values[key] = required ? number(input.id) : Number(input?.value || 0);
        });
        return values;
    }
    function updateSolid() {
        const definition = solidDefinitions[solidSelect.value];
        createFields("geometry-solid-inputs", definition.fields, "geometry-solid");
        formula("geometry-solid-formula", definition.formula);
        visuals.solid?.(solidSelect.value, solidValues(false));
    }
    solidSelect.addEventListener("change", updateSolid);
    document.getElementById("geometry-solid-inputs").addEventListener("input", () => visuals.solid?.(solidSelect.value, solidValues(false)));
    document.getElementById("geometry-solid-calculate").addEventListener("click", () => {
        try {
            const value = solidValues();
            const result = parse(module.geometrySolid(solidSelect.value, value.a || 0, value.b || 0, value.c || 0), "geometry-solid-result");
            if (result) render("geometry-solid-result", [["Oplošje", `${format(result.surfaceArea)} kvadratnih jedinica`, true], ["Obujam", `${format(result.volume)} kubnih jedinica`]]);
        } catch (error) { fail("geometry-solid-result", error.message); }
    });
    updateSolid();

    function coordinateValues() {
        return [number("geometry-x1"), number("geometry-y1"), number("geometry-x2"), number("geometry-y2")];
    }
    function drawCoordinates() {
        try { visuals.coordinates?.(...coordinateValues()); } catch (_) { }
    }
    ["geometry-x1", "geometry-y1", "geometry-x2", "geometry-y2"].forEach((id) => document.getElementById(id).addEventListener("input", drawCoordinates));
    document.getElementById("geometry-distance-calculate").addEventListener("click", () => {
        try {
            const result = parse(module.geometryCoordinates("distance", ...coordinateValues()), "geometry-coordinate-result");
            if (result) render("geometry-coordinate-result", [["Udaljenost |AB|", `${format(result.distance)} jedinica`, true]]);
        } catch (error) { fail("geometry-coordinate-result", error.message); }
    });
    document.getElementById("geometry-midpoint-calculate").addEventListener("click", () => {
        try {
            const result = parse(module.geometryCoordinates("midpoint", ...coordinateValues()), "geometry-coordinate-result");
            if (result) render("geometry-coordinate-result", [["Polovište S", `(${format(result.x)}, ${format(result.y)})`, true]]);
        } catch (error) { fail("geometry-coordinate-result", error.message); }
    });
    drawCoordinates();

    if (typeof setupGeometryPractice === "function") setupGeometryPractice(module, { render, fail, format });
}
