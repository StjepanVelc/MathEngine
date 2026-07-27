// MathEngine web frontend — placeholder dok se ne doda Emscripten/WASM build.
//
// Plan:
//  1. core/ se builda Emscripten toolchainom -> generira aksiomat.js + aksiomat.wasm
//  2. Ovdje se učita modul preko createAksiomatModule()
//  3. Baza podataka: SQLite u browseru (sql.js / službeni SQLite WASM), pohrana u IndexedDB/OPFS.

async function init() {
    if (typeof createAksiomatModule !== "function") {
        // WASM modul još nije priključen — ostaje placeholder poruka.
        return;
    }

    const module = await createAksiomatModule();

    document.getElementById("status").hidden = true;
    const demo = document.getElementById("demo");
    demo.hidden = false;

    const p = document.getElementById("var-p");
    const q = document.getElementById("var-q");
    const result = document.getElementById("result");

    function update() {
        result.textContent = module.demoEvaluate(p.checked, q.checked) ? "T" : "F";
    }

    p.addEventListener("change", update);
    q.addEventListener("change", update);
    update();
}

init();
