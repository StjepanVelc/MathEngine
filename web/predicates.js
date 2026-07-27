// Predikatna logika — parsiranje, interpretacija nad konačnom domenom i evaluacija.
// Očekuje WASM izvoze: predicateToString, predicateEvaluate.
// setupPalette dolazi iz app.js (učitava se nakon ove skripte, poziv ide iz init()).

function setupPredicates(module) {
    const input = document.getElementById("pred-input");
    const parsed = document.getElementById("pred-parsed");
    const domainInput = document.getElementById("pred-domain");
    const factsInput = document.getElementById("pred-facts");
    const evalBtn = document.getElementById("pred-eval");
    const result = document.getElementById("pred-result");

    setupPalette(document.querySelector('.symbol-palette[data-target="pred-input"]'), () => updateParsed());

    function updateParsed() {
        const formula = input.value.trim();
        if (!formula) {
            parsed.textContent = "";
            parsed.classList.remove("error");
            return;
        }
        const pretty = module.predicateToString(formula);
        parsed.textContent = pretty.startsWith("GRESKA:") ? pretty : "Formula: " + pretty;
        parsed.classList.toggle("error", pretty.startsWith("GRESKA:"));
    }

    function evaluate() {
        const formula = input.value.trim();
        if (!formula) {
            result.textContent = "—";
            result.classList.remove("error");
            return;
        }
        // Ukloni razmake iz domene i činjenica radi jednostavnijeg unosa.
        const domain = domainInput.value.replace(/\s+/g, "");
        const facts = factsInput.value.replace(/\s+/g, "");
        const text = module.predicateEvaluate(formula, domain, facts);
        result.textContent = text;
        result.classList.toggle("error", text.startsWith("GRESKA:"));
    }

    input.addEventListener("input", updateParsed);
    evalBtn.addEventListener("click", evaluate);
    [input, domainInput, factsInput].forEach((el) =>
        el.addEventListener("keydown", (e) => {
            if (e.key === "Enter") evaluate();
        })
    );
}
