// MathEngine web frontend — poglavlja (Logika, Aritmetika) pogonjena WASM jezgrom.
//
// Baza podataka: SQLite u browseru (sql.js / službeni SQLite WASM) — planirano.

function setupChapters() {
    document.querySelectorAll(".chapter-toggle").forEach((btn) => {
        btn.addEventListener("click", () => {
            const body = btn.nextElementSibling;
            const open = body.hidden;
            body.hidden = !open;
            btn.setAttribute("aria-expanded", String(open));
            btn.textContent = btn.textContent.replace(open ? "▸" : "▾", open ? "▾" : "▸");
        });
    });
}

// Pod-poglavlja unutar Logike — tri dugmeta, otvoreno najviše jedno.
function setupSubchapters() {
    const buttons = document.querySelectorAll(".subchapter-toggle");
    buttons.forEach((btn) => {
        btn.addEventListener("click", () => {
            const panel = document.getElementById(btn.dataset.panel);
            const open = panel.hidden;
            document.querySelectorAll(".subchapter-panel").forEach((p) => (p.hidden = true));
            buttons.forEach((b) => b.classList.remove("active"));
            if (open) {
                panel.hidden = false;
                btn.classList.add("active");
            }
        });
    });
}

// Paleta simbola — umeće znak na poziciju kursora u pripadajući input
// (određen atributom data-target palete). onInsert se poziva nakon umetanja.
function setupPalette(palette, onInsert) {
    const input = document.getElementById(palette.dataset.target);
    palette.querySelectorAll("button").forEach((btn) => {
        btn.addEventListener("click", () => {
            const symbol = btn.dataset.symbol;
            const start = input.selectionStart ?? input.value.length;
            const end = input.selectionEnd ?? input.value.length;
            input.value = input.value.slice(0, start) + symbol + input.value.slice(end);
            input.focus();
            input.setSelectionRange(start + symbol.length, start + symbol.length);
            if (onInsert) onInsert();
        });
    });
}

function setupLogic(module) {
    const input = document.getElementById("logic-input");
    const parsed = document.getElementById("logic-parsed");
    const varsBox = document.getElementById("logic-vars");
    const result = document.getElementById("logic-result");
    const tableBtn = document.getElementById("truth-table-btn");
    const tableBox = document.getElementById("truth-table-box");
    const classificationBox = document.getElementById("logic-classification");
    const formsBox = document.getElementById("normal-forms-box");
    const values = new Map();

    setupPalette(document.querySelector('.symbol-palette[data-target="logic-input"]'), () => update());

    function evaluate() {
        const vars = [...values.entries()].map(([k, v]) => `${k}=${v ? 1 : 0}`).join(",");
        const text = module.logicEvaluate(input.value.trim(), vars);
        result.textContent = text;
        result.classList.toggle("error", text.startsWith("GRESKA:"));
    }

    function update() {
        const formula = input.value.trim();
        tableBox.innerHTML = "";
        if (!formula) {
            parsed.textContent = "";
            varsBox.innerHTML = "";
            result.textContent = "—";
            result.classList.remove("error");
            classificationBox.textContent = "";
            formsBox.hidden = true;
            return;
        }

        const pretty = module.logicToString(formula);
        if (pretty.startsWith("GRESKA:")) {
            parsed.textContent = pretty;
            parsed.classList.add("error");
            varsBox.innerHTML = "";
            result.textContent = "—";
            classificationBox.textContent = "";
            formsBox.hidden = true;
            return;
        }
        parsed.textContent = "Formula: " + pretty;
        parsed.classList.remove("error");

        const classificationText = module.logicClassification(formula);
        const formsText = module.logicNormalForms(formula);
        if (!classificationText.startsWith("GRESKA:")) {
            const classification = JSON.parse(classificationText);
            classificationBox.textContent = classification.tautology
                ? "Klasifikacija: tautologija (uvijek istinita)."
                : classification.contradiction
                    ? "Klasifikacija: kontradikcija (uvijek neistinita)."
                    : "Klasifikacija: zadovoljiva i kontingentna formula.";
        }
        if (!formsText.startsWith("GRESKA:")) {
            const forms = JSON.parse(formsText);
            document.getElementById("logic-nnf").textContent = forms.nnf;
            document.getElementById("logic-cnf").textContent = forms.transformedCnf;
            document.getElementById("logic-canonical-cnf").textContent = forms.canonicalCnf;
            document.getElementById("logic-dnf").textContent = forms.transformedDnf;
            document.getElementById("logic-canonical-dnf").textContent = forms.canonicalDnf;
            formsBox.hidden = false;
        } else {
            formsBox.hidden = true;
            classificationBox.textContent += ` Normalne forme nisu dostupne: ${formsText}`;
        }

        // Dinamički checkboxovi za varijable (čuvaj postojeće vrijednosti).
        const names = module.logicVariables(formula).split(",").filter(Boolean);
        for (const key of [...values.keys()]) {
            if (!names.includes(key)) values.delete(key);
        }
        varsBox.innerHTML = "";
        for (const name of names) {
            if (!values.has(name)) values.set(name, false);
            const label = document.createElement("label");
            const box = document.createElement("input");
            box.type = "checkbox";
            box.checked = values.get(name);
            box.addEventListener("change", () => {
                values.set(name, box.checked);
                evaluate();
            });
            label.append(box, " " + name);
            varsBox.append(label);
        }

        evaluate();
    }

    function renderTruthTable() {
        const formula = input.value.trim();
        tableBox.innerHTML = "";
        if (!formula) return;
        const text = module.truthTable(formula);
        if (text.startsWith("GRESKA:")) {
            tableBox.innerHTML = `<p class="parsed error">${text}</p>`;
            return;
        }
        const data = JSON.parse(text);
        const table = document.createElement("table");
        table.className = "truth-table";
        const header = document.createElement("tr");
        for (const name of data.variables) {
            const th = document.createElement("th");
            th.textContent = name;
            header.append(th);
        }
        const thResult = document.createElement("th");
        thResult.textContent = module.logicToString(formula);
        header.append(thResult);
        table.append(header);
        for (const row of data.rows) {
            const tr = document.createElement("tr");
            for (const v of row.values) {
                const td = document.createElement("td");
                td.textContent = v ? "T" : "F";
                tr.append(td);
            }
            const tdResult = document.createElement("td");
            tdResult.textContent = row.result ? "T" : "F";
            tdResult.className = row.result ? "true-cell" : "false-cell";
            tr.append(tdResult);
            table.append(tr);
        }
        tableBox.append(table);
    }

    input.addEventListener("input", update);
    tableBtn.addEventListener("click", renderTruthTable);
}

function setupArithmetic(module) {
    const input = document.getElementById("arith-input");
    const button = document.getElementById("arith-eval");
    const result = document.getElementById("arith-result");

    function evaluate() {
        const expr = input.value.trim();
        if (!expr) {
            result.textContent = "—";
            return;
        }
        const text = module.evaluateArithmetic(expr);
        result.textContent = text;
        result.classList.toggle("error", text.startsWith("GRESKA:"));
    }

    button.addEventListener("click", evaluate);
    input.addEventListener("keydown", (e) => {
        if (e.key === "Enter") evaluate();
    });
}

async function init() {
    const status = document.getElementById("status");
    if (typeof createAksiomatModule !== "function") {
        status.innerHTML = "<p>WASM modul nije pronađen — pokreni WASM build (cmake --build out\\build\\wasm-release).</p>";
        return;
    }

    const module = await createAksiomatModule();

    status.hidden = true;
    document.querySelectorAll(".chapter").forEach((s) => (s.hidden = false));

    setupChapters();
    setupSubchapters();
    setupLogic(module);
    setupPredicates(module);
    setupFormalization(module);
    setupArithmetic(module);
}

init();
