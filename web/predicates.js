// Predikatna logika — automatski uređivač interpretacije nad konačnom domenom.
const MAX_INTERPRETATION_TUPLES = 256;

function setupPredicates(module) {
    const input = document.getElementById("pred-input");
    const parsed = document.getElementById("pred-parsed");
    const highlighted = document.getElementById("pred-highlighted");
    const domainInput = document.getElementById("pred-domain");
    const freeVariables = document.getElementById("pred-free-vars");
    const boundVariables = document.getElementById("pred-bound-vars");
    const editor = document.getElementById("pred-interpretation");
    const evalBtn = document.getElementById("pred-eval");
    const result = document.getElementById("pred-result");
    let predicateInfo = [];

    setupPalette(document.querySelector('.symbol-palette[data-target="pred-input"]'), refresh);

    function domainElements() {
        const raw = domainInput.value.trim();
        if (!raw) return [];
        const elements = raw.split(",").map((value) => value.trim());
        if (elements.some((value) => !value)) throw new Error("Domena sadrži prazan element.");
        if (new Set(elements).size !== elements.length) throw new Error("Elementi domene moraju biti jedinstveni.");
        return elements;
    }

    function tuples(elements, arity) {
        let result = [[]];
        for (let i = 0; i < arity; ++i) {
            result = result.flatMap((prefix) => elements.map((element) => [...prefix, element]));
        }
        return result;
    }

    function renderEditor() {
        editor.innerHTML = "";
        result.textContent = "—";
        if (!predicateInfo.length) return;
        let elements;
        try {
            elements = domainElements();
        } catch (error) {
            editor.innerHTML = `<p class="parsed error">${error.message}</p>`;
            return;
        }
        if (!elements.length) {
            editor.innerHTML = '<p class="parsed">Upiši domenu kako bi se izgradila interpretacija.</p>';
            return;
        }
        const total = predicateInfo.reduce((sum, item) => sum + elements.length ** item.arity, 0);
        if (total > MAX_INTERPRETATION_TUPLES) {
            editor.innerHTML = `<p class="parsed error">Interpretacija bi imala ${total} n-torki; dopušteno je najviše ${MAX_INTERPRETATION_TUPLES}. Smanji domenu ili arnost.</p>`;
            return;
        }
        for (const predicate of predicateInfo) {
            const fieldset = document.createElement("fieldset");
            fieldset.className = "predicate-editor";
            const legend = document.createElement("legend");
            legend.textContent = `${predicate.name}/${predicate.arity}`;
            fieldset.append(legend);
            const grid = document.createElement("div");
            grid.className = predicate.arity === 2 ? "tuple-grid binary" : "tuple-grid";
            for (const tuple of tuples(elements, predicate.arity)) {
                const label = document.createElement("label");
                const checkbox = document.createElement("input");
                checkbox.type = "checkbox";
                checkbox.dataset.fact = `${predicate.name}(${tuple.join(",")})`;
                label.append(checkbox, ` (${tuple.join(", ")})`);
                grid.append(label);
            }
            fieldset.append(grid);
            editor.append(fieldset);
        }
    }

    function refresh() {
        const formula = input.value.trim();
        predicateInfo = [];
        freeVariables.textContent = "";
        boundVariables.textContent = "";
        highlighted.textContent = "";
        if (!formula) {
            parsed.textContent = "";
            parsed.classList.remove("error");
            editor.innerHTML = "";
            return;
        }
        const pretty = module.predicateToString(formula);
        parsed.textContent = pretty.startsWith("GRESKA:") ? pretty : "Formula: " + pretty;
        parsed.classList.toggle("error", pretty.startsWith("GRESKA:"));
        if (pretty.startsWith("GRESKA:")) {
            editor.innerHTML = "";
            return;
        }
        const description = module.predicatePredicates(formula);
        if (description.startsWith("GRESKA:")) {
            parsed.textContent = description;
            parsed.classList.add("error");
            return;
        }
        let data;
        try {
            data = JSON.parse(description);
        } catch {
            parsed.textContent = "GRESKA: Neispravan odgovor predikatnog modula.";
            parsed.classList.add("error");
            editor.innerHTML = "";
            return;
        }
        if (!Array.isArray(data.predicates) || !Array.isArray(data.freeVariables) ||
            !Array.isArray(data.boundVariables)) {
            parsed.textContent = "GRESKA: Nepotpun opis predikatne formule.";
            parsed.classList.add("error");
            editor.innerHTML = "";
            return;
        }
        predicateInfo = data.predicates;
        freeVariables.textContent = data.freeVariables.length
            ? "Slobodne varijable/konstante: " + data.freeVariables.join(", ")
            : "Formula je zatvorena (nema slobodnih varijabli).";
        boundVariables.textContent = data.boundVariables.length
            ? "Vezane varijable: " + data.boundVariables.join(", ")
            : "Formula nema vezanih varijabli.";

        highlighted.append("Označeni prikaz: ");
        const names = new Set([...data.freeVariables, ...data.boundVariables]);
        const tokenPattern = /[A-Za-z0-9_]+|[^A-Za-z0-9_]+/g;
        for (const token of pretty.match(tokenPattern) || []) {
            if (!names.has(token)) {
                highlighted.append(token);
                continue;
            }
            const span = document.createElement("span");
            span.textContent = token;
            span.className = data.boundVariables.includes(token) ? "bound-variable" : "free-variable";
            highlighted.append(span);
        }
        renderEditor();
    }

    function evaluate() {
        const formula = input.value.trim();
        if (!formula) return;
        let domain;
        try {
            domain = domainElements();
        } catch (error) {
            result.textContent = "GRESKA: " + error.message;
            result.classList.add("error");
            return;
        }
        if (!domain.length) {
            result.textContent = "GRESKA: Domena ne smije biti prazna.";
            result.classList.add("error");
            return;
        }
        const facts = [...editor.querySelectorAll('input[data-fact]:checked')]
            .map((checkbox) => checkbox.dataset.fact)
            .join(";");
        const text = module.predicateEvaluate(formula, domain.join(","), facts);
        result.textContent = text;
        result.classList.toggle("error", text.startsWith("GRESKA:"));
    }

    input.addEventListener("input", refresh);
    domainInput.addEventListener("input", renderEditor);
    evalBtn.addEventListener("click", evaluate);
    [input, domainInput].forEach((element) => element.addEventListener("keydown", (event) => {
        if (event.key === "Enter") evaluate();
    }));
}
