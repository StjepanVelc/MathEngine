// Algebra alati pogonjeni zasebnim WASM servisima.

function setupAlgebra(module) {
    function schoolNotation(value) {
        if (value === null || value === undefined) return "—";
        return String(value)
            .replace(/\^(-?\d+)/g, (_, exponent) => exponent.replace(/[-0-9]/g, (digit) => ({
                "-": "⁻", "0": "⁰", "1": "¹", "2": "²", "3": "³", "4": "⁴",
                "5": "⁵", "6": "⁶", "7": "⁷", "8": "⁸", "9": "⁹"
            })[digit]))
            .replace(/\s*\*\s*(?=[A-Za-z(])/g, "")
            .replace(/\)\s*\*\s*(?=[A-Za-z(])/g, ")")
            .replace(/\s+/g, " ")
            .trim();
    }

    function appendNumberLine(box, data) {
        const figure = document.createElement("figure");
        figure.className = "number-line";
        const caption = document.createElement("figcaption");
        caption.textContent = `Brojevni pravac: ${data.interval}`;
        const namespace = "http://www.w3.org/2000/svg";
        const svg = document.createElementNS(namespace, "svg");
        svg.setAttribute("viewBox", "0 0 640 100");
        svg.setAttribute("role", "img");
        svg.setAttribute("aria-label", caption.textContent);
        const addLine = (x1, y1, x2, y2, className) => {
            const line = document.createElementNS(namespace, "line");
            Object.entries({ x1, y1, x2, y2 }).forEach(([key, value]) => line.setAttribute(key, value));
            line.setAttribute("class", className);
            svg.append(line);
        };
        addLine(35, 50, 605, 50, "number-line-axis");
        if (data.type === "interval") {
            const pointX = 320;
            const right = data.relation === ">" || data.relation === ">=";
            addLine(right ? pointX : 45, 50, right ? 595 : pointX, 50, "number-line-solution");
            const arrow = document.createElementNS(namespace, "polygon");
            arrow.setAttribute("points", right ? "605,50 588,40 588,60" : "35,50 52,40 52,60");
            arrow.setAttribute("class", "number-line-arrow");
            svg.append(arrow);
            const circle = document.createElementNS(namespace, "circle");
            circle.setAttribute("cx", pointX);
            circle.setAttribute("cy", "50");
            circle.setAttribute("r", "9");
            circle.setAttribute("class", data.relation.includes("=") ? "number-line-point closed" : "number-line-point open");
            svg.append(circle);
            const label = document.createElementNS(namespace, "text");
            label.setAttribute("x", pointX);
            label.setAttribute("y", "82");
            label.setAttribute("text-anchor", "middle");
            label.textContent = data.boundary;
            svg.append(label);
        } else if (data.type === "all") {
            addLine(45, 50, 595, 50, "number-line-solution");
            const label = document.createElementNS(namespace, "text");
            label.setAttribute("x", "320"); label.setAttribute("y", "82"); label.setAttribute("text-anchor", "middle");
            label.textContent = "svi realni brojevi"; svg.append(label);
        } else {
            const label = document.createElementNS(namespace, "text");
            label.setAttribute("x", "320"); label.setAttribute("y", "58"); label.setAttribute("text-anchor", "middle");
            label.textContent = "∅  nema rješenja"; svg.append(label);
        }
        figure.append(caption, svg);
        box.append(figure);
    }

    function appendRows(box, rows) {
        const grid = document.createElement("div");
        grid.className = "result-grid";
        rows.forEach(([label, value, primary = false]) => {
            const row = document.createElement("div");
            row.className = `result-row${primary ? " result-primary" : ""}`;
            const name = document.createElement("span");
            name.textContent = label;
            const content = document.createElement("strong");
            content.textContent = schoolNotation(value);
            row.append(name, content);
            grid.append(row);
        });
        box.append(grid);
    }

    function appendMethod(box, title, steps) {
        const details = document.createElement("details");
        details.className = "method-card";
        const summary = document.createElement("summary");
        summary.textContent = title;
        const list = document.createElement("ol");
        list.className = "solution-steps";
        steps.forEach((step) => {
            const item = document.createElement("li");
            item.textContent = schoolNotation(step);
            list.append(item);
        });
        details.append(summary, list);
        box.append(details);
    }

    const toggles = document.querySelectorAll(".algebra-toggle");
    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".algebra-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) {
            panel.hidden = false;
            button.classList.add("active");
            if (panel.id === "algebra-functions") window.dispatchEvent(new Event("resize"));
        }
    }));

    function render(id, text, summary) {
        const box = document.getElementById(id);
        box.innerHTML = "";
        box.classList.toggle("error", text.startsWith("GRESKA:"));
        if (text.startsWith("GRESKA:")) {
            box.textContent = text;
            return null;
        }
        const data = JSON.parse(text);
        if (summary) {
            const heading = document.createElement("strong");
            heading.className = "result-summary";
            heading.textContent = schoolNotation(summary(data));
            box.append(heading);
        }
        if (data.steps) {
            const list = document.createElement("ol");
            list.className = "solution-steps";
            data.steps.forEach((step) => {
                const item = document.createElement("li");
                item.textContent = schoolNotation(step);
                list.append(item);
            });
            box.append(list);
        }
        return data;
    }

    document.getElementById("algebra-simplify").addEventListener("click", () => {
        render("algebra-expression-result", module.algebraSimplify(
            document.getElementById("algebra-expression-input").value.trim()), (data) => `Rezultat: ${data.result}`);
    });
    document.getElementById("algebra-equation-solve").addEventListener("click", () => {
        render("algebra-equation-result", module.algebraSolveEquation(
            document.getElementById("algebra-equation-input").value.trim()), (data) =>
            data.type === "unique" ? `Jedinstveno rješenje: x = ${data.value}` :
                data.type === "infinite" ? "Beskonačno mnogo rješenja" : "Nema rješenja");
    });
    document.getElementById("algebra-inequality-solve").addEventListener("click", () => {
        const data = render("algebra-inequality-result", module.algebraSolveInequality(
            document.getElementById("algebra-inequality-input").value.trim()), (data) => `Skup rješenja: ${data.interval}`);
        if (data) appendNumberLine(document.getElementById("algebra-inequality-result"), data);
    });
    document.getElementById("algebra-system-solve").addEventListener("click", () => {
        const data = render("algebra-system-result", module.algebraSolveSystem(
            document.getElementById("algebra-system-first").value.trim(),
            document.getElementById("algebra-system-second").value.trim()), (data) =>
            data.type === "unique" ? `Rješenje: (x, y) = (${data.x}, ${data.y})` :
                data.type === "infinite" ? "Beskonačno mnogo rješenja" : "Sustav nema rješenja");
        if (data?.methods) {
            const box = document.getElementById("algebra-system-result");
            box.querySelector(".solution-steps")?.remove();
            appendMethod(box, "Cramerovo pravilo", data.methods.cramer);
            appendMethod(box, "Metoda supstitucije", data.methods.substitution);
            appendMethod(box, "Metoda eliminacije", data.methods.elimination);
        }
    });
    document.getElementById("algebra-polynomial-analyze").addEventListener("click", () => {
        const text = module.algebraAnalyzePolynomial(document.getElementById("algebra-polynomial-input").value.trim());
        const data = render("algebra-polynomial-result", text);
        if (!data) return;
        appendRows(document.getElementById("algebra-polynomial-result"), [
            ["Sređeni oblik", data.normalized, true],
            ["Stupanj", data.degree],
            ["Derivacija", data.derivative],
            ["Nultočke", data.roots.length ? data.roots.join(", ") : "nema realnih"],
            ["Diskriminanta", data.discriminant === null ? "nije primjenjivo" : data.discriminant],
            ["Vrh parabole", data.vertex ? `(${data.vertex.x}, ${data.vertex.y})` : "nije primjenjivo"],
            ["Faktorizirani oblik", data.factorized ?? "nema faktorizacije nad realnim brojevima"]
        ]);
    });

    setupAlgebraGraph(module);
}
