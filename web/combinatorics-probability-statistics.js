function setupCombinatoricsProbabilityStatistics(module) {
    const visuals = window.CombinatoricsProbabilityStatisticsVisuals || {};
    const toggles = [...document.querySelectorAll(".combinatorics-probability-statistics-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".combinatorics-probability-statistics-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); visuals.refresh?.(panel.id); } }));

    function number(id) { const input = document.getElementById(id); const value = Number(input.value); if (!input || input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj."); return value; }
    function dataset(id) { const input = document.getElementById(id); const text = (input.value || "").trim(); if (!text) throw new Error("Upiši barem jedan podatak odvojen zarezima."); return text; }
    function parse(text, boxId) { if (text.startsWith("GRESKA:")) { fail(boxId, text); return null; } return JSON.parse(text); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((text, index) => { const item = document.createElement("div"); item.className = "combinatorics-probability-statistics-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = text; item.append(badge, content); box.append(item); }); }

    // Prebrojavanje
    const countingMode = document.getElementById("cps-counting-mode");
    function countingFields() {
        const root = document.getElementById("cps-counting-fields"); root.innerHTML = "";
        const needsK = countingMode.value !== "factorial";
        const definitions = needsK ? [["n", "n", 5], ["k", "k", 2]] : [["n", "n", 5]];
        definitions.forEach(([key, label, value]) => { const wrapper = document.createElement("label"); wrapper.textContent = label; const input = document.createElement("input"); input.type = "number"; input.step = "1"; input.value = value; input.id = `cps-counting-${key}`; wrapper.append(input); root.append(wrapper); });
    }
    function calculateCounting() {
        try {
            const mode = countingMode.value;
            const n = number("cps-counting-n");
            const k = mode === "factorial" ? 0 : number("cps-counting-k");
            const result = parse(module.combinatoricsCounting(mode, n, k), "cps-counting-result");
            if (!result) return;
            render("cps-counting-result", [["Rezultat", format(result.value), true]]);
            steps("cps-counting-steps", mode === "factorial"
                ? ["n! je umnožak svih prirodnih brojeva od 1 do n.", "Za n=0 vrijedi 0!=1 po dogovoru."]
                : mode === "permutations"
                    ? ["Permutacije bez ponavljanja: P(n,k) = n! / (n-k)!.", "Poredak elemenata je bitan, elementi se ne ponavljaju."]
                    : mode === "permutationsWithRepetition"
                        ? ["Permutacije s ponavljanjem: n^k.", "Svaki od k izbora ima n mogućnosti neovisno o prethodnima."]
                        : mode === "combinations"
                            ? ["Kombinacije bez ponavljanja: C(n,k) = n! / (k!(n-k)!).", "Poredak elemenata nije bitan."]
                            : ["Kombinacije s ponavljanjem: C(n+k-1,k).", "Elementi se mogu ponavljati, a poredak nije bitan."]);
        } catch (error) { fail("cps-counting-result", error); }
    }
    countingMode.addEventListener("change", () => { countingFields(); calculateCounting(); });
    document.getElementById("cps-counting-calculate").addEventListener("click", calculateCounting);
    countingFields();

    // Vjerojatnost
    const probabilityMode = document.getElementById("cps-probability-mode");
    function probabilityFields() {
        const root = document.getElementById("cps-probability-fields"); root.innerHTML = "";
        const definitions = probabilityMode.value === "classical" ? [["first", "Povoljni ishodi", 3], ["second", "Ukupno ishoda", 6]]
            : probabilityMode.value === "complement" ? [["first", "P(A)", 0.3]]
                : probabilityMode.value === "union" ? [["first", "P(A)", 0.5], ["second", "P(B)", 0.4], ["third", "P(A i B)", 0.2]]
                    : probabilityMode.value === "conditional" ? [["first", "P(A i B)", 0.2], ["second", "P(B)", 0.5]]
                        : [["first", "P(A)", 0.5], ["second", "P(B)", 0.4]];
        definitions.forEach(([key, label, value]) => { const wrapper = document.createElement("label"); wrapper.textContent = label; const input = document.createElement("input"); input.type = "number"; input.step = "any"; input.value = value; input.id = `cps-probability-${key}`; wrapper.append(input); root.append(wrapper); });
    }
    function calculateProbability() {
        try {
            const mode = probabilityMode.value;
            const first = number("cps-probability-first");
            const second = document.getElementById("cps-probability-second") ? number("cps-probability-second") : 0;
            const third = document.getElementById("cps-probability-third") ? number("cps-probability-third") : 0;
            const result = parse(module.combinatoricsProbability(mode, first, second, third), "cps-probability-result");
            if (!result) return;
            render("cps-probability-result", [["Vjerojatnost", format(result.value), true]]);
            steps("cps-probability-steps", mode === "classical"
                ? ["Klasična vjerojatnost: P(A) = povoljni / ukupni ishodi.", "Sve ishode smatramo jednako mogućima."]
                : mode === "complement"
                    ? ["Komplement: P(A') = 1 − P(A).", "Zbroj vjerojatnosti događaja i njegovog komplementa uvijek je 1."]
                    : mode === "union"
                        ? ["Unija: P(A∪B) = P(A) + P(B) − P(A∩B).", "Oduzimamo presjek da se ne broji dvaput."]
                        : mode === "conditional"
                            ? ["Uvjetna vjerojatnost: P(A|B) = P(A∩B) / P(B).", "Promatramo vjerojatnost događaja A uz uvjet da se dogodio B."]
                            : ["Nezavisni događaji: P(A∩B) = P(A)·P(B).", "Nastup jednog događaja ne utječe na vjerojatnost drugog."]);
        } catch (error) { fail("cps-probability-result", error); }
    }
    probabilityMode.addEventListener("change", () => { probabilityFields(); calculateProbability(); });
    document.getElementById("cps-probability-calculate").addEventListener("click", calculateProbability);
    probabilityFields();

    // Deskriptivna statistika
    function calculateStatistics() {
        try {
            const values = dataset("cps-statistics-dataset");
            const result = parse(module.combinatoricsStatistics(values), "cps-statistics-result");
            if (!result) return;
            const rows = [
                ["Aritmetička sredina", format(result.mean), true],
                ["Medijan", format(result.median)],
                ["Mod", result.mode.length ? result.mode.map(format).join(", ") : "nema"],
                ["Varijanca", format(result.variance)],
                ["Standardna devijacija", format(result.standardDeviation)],
                ["Raspon", format(result.range)],
            ];
            if (result.q1 !== undefined) rows.push(["Q1 / Q2 / Q3", `${format(result.q1)} / ${format(result.q2)} / ${format(result.q3)}`], ["Interkvartilni raspon", format(result.interquartileRange)]);
            render("cps-statistics-result", rows);
            steps("cps-statistics-steps", ["Aritmetička sredina je zbroj podataka podijeljen s njihovim brojem.", "Varijanca mjeri prosječno kvadratno odstupanje od sredine, a standardna devijacija je njen korijen.", "Kvartili dijele sortirani skup podataka na četiri jednaka dijela."]);
        } catch (error) { fail("cps-statistics-result", error); }
    }
    document.getElementById("cps-statistics-calculate").addEventListener("click", calculateStatistics);

    // Vizualizacija podataka
    function calculateVisualization() {
        try {
            const values = dataset("cps-visualization-dataset");
            const binCount = number("cps-visualization-bins");
            const result = parse(module.combinatoricsVisualization(values, binCount), "cps-visualization-result");
            if (!result) return;
            render("cps-visualization-result", [["Broj razreda", result.bins.length, true]]);
            steps("cps-visualization-steps", ["Podaci se dijele u jednake razrede (bins) između minimuma i maksimuma.", "Histogram prikazuje frekvenciju (broj podataka) po razredu."]);
            visuals.histogram?.("cps-visualization-visual", result.bins);
        } catch (error) { fail("cps-visualization-result", error); }
    }
    document.getElementById("cps-visualization-calculate").addEventListener("click", calculateVisualization);

    setupCombinatoricsProbabilityStatisticsPractice({ root: document.getElementById("cps-practice-root"), format });
}
