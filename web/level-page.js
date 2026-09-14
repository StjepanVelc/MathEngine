// MathEngine — zajednička inicijalizacija za stranice pojedine razine
// (osnovna-skola.html, srednja-skola.html, fakultet.html).
//
// Svaka stranica učitava samo skripte poglavlja koja su joj potrebna;
// ova skripta primjenjuje filtriranje po razini, otvara/zatvara poglavlja
// i pokreće WASM modul te poziva samo one setup funkcije koje postoje.

function setupPalette(palette, onInsert) {
    if (!palette) return;
    const input = document.getElementById(palette.dataset.target);
    if (!input) return;
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

function supportsLevel(element, level) {
    return !element.dataset.levels || element.dataset.levels.split(/\s+/).includes(level);
}

function applyLevelFilter(level) {
    document.querySelectorAll(".chapter").forEach((chapter) => {
        chapter.hidden = !supportsLevel(chapter, level);
    });
    document.querySelectorAll(
        ".subchapter-toggle, .arithmetic-toggle, .algebra-toggle, .geometry-toggle, .trigonometry-toggle, .sequences-toggle, .analytic-geometry-toggle, .exponential-logarithmic-toggle, .calculus-basics-toggle, .mathematical-analysis-toggle, .analytic-algebra-toggle, .discrete-math-toggle, .probability-statistics-toggle, .complex-numbers-toggle"
    ).forEach((button) => {
        const available = supportsLevel(button, level);
        button.hidden = !available;
        if (!available) button.classList.remove("active");
    });
    document.querySelectorAll(
        ".subchapter-panel, .arithmetic-panel, .algebra-panel, .geometry-panel, .trigonometry-panel, .sequences-panel, .analytic-geometry-panel, .exponential-logarithmic-panel, .calculus-basics-panel, .mathematical-analysis-panel, .analytic-algebra-panel, .discrete-math-panel, .probability-statistics-panel, .complex-numbers-panel"
    ).forEach((panel) => {
        panel.hidden = !supportsLevel(panel, level);
    });
}

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

function callIfDefined(name, ...args) {
    if (typeof window[name] === "function") {
        window[name](...args);
    }
}

async function initLevelPage() {
    const level = document.body.dataset.learningLevel;
    applyLevelFilter(level);
    setupChapters();
    setupSubchapters();

    const status = document.getElementById("status");
    if (typeof createAksiomatModule !== "function") {
        if (status) status.innerHTML = "<p>WASM modul nije pronađen — pokreni WASM build (cmake --build out\\build\\wasm-release).</p>";
        return;
    }

    const module = await createAksiomatModule();
    if (status) status.hidden = true;

    callIfDefined("setupLogic", module);
    callIfDefined("setupPredicates", module);
    callIfDefined("setupFormalization", module);
    callIfDefined("setupArithmetic", module);
    callIfDefined("setupGeometry", module);
    callIfDefined("setupTrigonometry", module);
    callIfDefined("setupSequences", module);
    callIfDefined("setupAnalyticGeometry", module);
    callIfDefined("setupExponentialLogarithmic", module);
    callIfDefined("setupCombinatoricsProbabilityStatistics", module);
    callIfDefined("setupCalculusBasics", module);
    callIfDefined("setupMathematicalAnalysis", module);
    callIfDefined("setupAlgebra", module);
    callIfDefined("setupAnalyticAlgebra", module);
    callIfDefined("setupDiscreteMath", module);
    callIfDefined("setupProbabilityStatistics", module);
    callIfDefined("setupComplexNumbers", module);
}

initLevelPage();
