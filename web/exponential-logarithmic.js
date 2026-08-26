function setupExponentialLogarithmic(module) {
    const visuals = window.ExponentialLogarithmicVisuals || {};
    const toggles = [...document.querySelectorAll(".exponential-logarithmic-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".exponential-logarithmic-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); visuals.refresh?.(panel.id); } }));

    function number(id) { const input = document.getElementById(id); const value = Number(input.value); if (!input || input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj."); return value; }
    function parse(text, boxId) { if (text.startsWith("GRESKA:")) { fail(boxId, text); return null; } return JSON.parse(text); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((text, index) => { const item = document.createElement("div"); item.className = "exponential-logarithmic-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = text; item.append(badge, content); box.append(item); }); }

    // Potencije i korijeni
    const powersMode = document.getElementById("explog-powers-mode");
    function powersFields() {
        const root = document.getElementById("explog-powers-fields"); root.innerHTML = "";
        const definitions = powersMode.value === "power"
            ? [["base", "Baza", 2], ["exponent", "Eksponent", 10]]
            : [["base", "Radikand", 27], ["degree", "Stupanj korijena", 3]];
        definitions.forEach(([key, label, value]) => { const wrapper = document.createElement("label"); wrapper.textContent = label; const input = document.createElement("input"); input.type = "number"; input.step = "any"; input.value = value; input.id = `explog-powers-${key}`; wrapper.append(input); root.append(wrapper); });
    }
    function calculatePowers() {
        try {
            const mode = powersMode.value;
            const base = number("explog-powers-base");
            const secondary = mode === "power" ? number("explog-powers-exponent") : number("explog-powers-degree");
            const result = parse(module.exponentialLogPowersRoots(mode, base, mode === "power" ? secondary : 0, mode === "root" ? secondary : 0), "explog-powers-result");
            if (!result) return;
            render("explog-powers-result", [["Rezultat", format(result.value), true]]);
            steps("explog-powers-steps", mode === "power"
                ? ["Odredi bazu i eksponent.", "Pomnoži bazu sa sobom onoliko puta koliko iznosi eksponent (ili primijeni pravila za negativne/razlomljene eksponente).", "Provjeri je li rezultat konačan broj."]
                : ["Odredi radikand i stupanj korijena.", "Ako je stupanj neparan i radikand negativan, rezultat je negativan broj.", "Provjeri rezultat potenciranjem natrag na zadani stupanj."]);
        } catch (error) { fail("explog-powers-result", error); }
    }
    powersMode.addEventListener("change", () => { powersFields(); calculatePowers(); });
    document.getElementById("explog-powers-calculate").addEventListener("click", calculatePowers);
    powersFields();

    // Eksponencijalna funkcija
    function calculateExponential() {
        try {
            const initialValue = number("explog-exp-initial");
            const base = number("explog-exp-base");
            const xMin = number("explog-exp-xmin");
            const xMax = number("explog-exp-xmax");
            const result = parse(module.exponentialLogFunction(initialValue, base, xMin, xMax, 60), "explog-exp-result");
            if (!result) return;
            render("explog-exp-result", [["Vrsta", result.isGrowth ? "Rast" : "Pad", true], ["Stopa promjene", `${format(result.growthRatePercent)}%`]]);
            steps("explog-exp-steps", ["Funkcija oblika f(x)=a·bˣ.", "Ako je baza b>1, funkcija raste; ako je 0<b<1, funkcija pada.", "Graf uvijek prolazi kroz točku (0, a) i približava se osi x kao asimptoti."]);
            visuals.exponential?.("explog-exp-visual", result.samples);
        } catch (error) { fail("explog-exp-result", error); }
    }
    document.getElementById("explog-exp-calculate").addEventListener("click", calculateExponential);

    // Logaritamska funkcija
    function calculateLogarithm() {
        try {
            const base = number("explog-log-base");
            const xMin = number("explog-log-xmin");
            const xMax = number("explog-log-xmax");
            const result = parse(module.exponentialLogLogarithm(base, xMin, xMax, 60), "explog-log-result");
            if (!result) return;
            render("explog-log-result", [["Broj uzoraka", result.samples.length, true]]);
            steps("explog-log-steps", ["Funkcija oblika f(x)=log_b(x) definirana je samo za x>0.", "Graf ima vertikalnu asimptotu na osi y (x=0).", "Ako je baza b>1, funkcija raste; ako je 0<b<1, funkcija pada."]);
            visuals.logarithm?.("explog-log-visual", result.samples);
        } catch (error) { fail("explog-log-result", error); }
    }
    document.getElementById("explog-log-calculate").addEventListener("click", calculateLogarithm);

    // Jednadžbe
    const equationKind = document.getElementById("explog-equation-kind");
    function calculateEquation() {
        try {
            const kind = equationKind.value;
            const base = number("explog-equation-base");
            const coefficient = number("explog-equation-coefficient");
            const target = number("explog-equation-target");
            const result = parse(module.exponentialLogEquation(kind, base, coefficient, target), "explog-equation-result");
            if (!result) return;
            render("explog-equation-result", [["x =", format(result.solution), true]]);
            steps("explog-equation-steps", kind === "exponential"
                ? ["Podijeli obje strane s koeficijentom.", "Primijeni logaritam po zadanoj bazi na obje strane.", "Rezultat je x = log_baza(cilj/koeficijent)."]
                : ["Podijeli obje strane s koeficijentom.", "Prebaci jednadžbu u eksponencijalni oblik.", "Rezultat je x = baza^(cilj/koeficijent)."]);
        } catch (error) { fail("explog-equation-result", error); }
    }
    equationKind.addEventListener("change", calculateEquation);
    document.getElementById("explog-equation-calculate").addEventListener("click", calculateEquation);

    // Primjene
    const applicationKind = document.getElementById("explog-application-kind");
    const applicationLabels = { radioactiveDecay: "Preostala količina", phLevel: "pH vrijednost", richterMagnitude: "Magnituda", soundIntensityDecibels: "Razina zvuka (dB)" };
    function applicationFields() {
        const root = document.getElementById("explog-application-fields"); root.innerHTML = "";
        const definitions = applicationKind.value === "radioactiveDecay" ? [["first", "Početna količina", 80], ["second", "Poluživot", 5], ["third", "Proteklo vrijeme", 10]]
            : applicationKind.value === "phLevel" ? [["first", "Koncentracija H⁺ (mol/L)", 0.0000001]]
            : applicationKind.value === "richterMagnitude" ? [["first", "Omjer amplituda", 10000]]
            : [["first", "Intenzitet", 0.000001], ["second", "Referentni intenzitet", 0.000000000001]];
        definitions.forEach(([key, label, value]) => { const wrapper = document.createElement("label"); wrapper.textContent = label; const input = document.createElement("input"); input.type = "number"; input.step = "any"; input.value = value; input.id = `explog-application-${key}`; wrapper.append(input); root.append(wrapper); });
    }
    function calculateApplication() {
        try {
            const kind = applicationKind.value;
            const values = ["first", "second", "third"].map((key) => document.getElementById(`explog-application-${key}`) ? number(`explog-application-${key}`) : 0);
            const result = parse(module.exponentialLogApplication(kind, ...values), "explog-application-result");
            if (!result) return;
            render("explog-application-result", [[applicationLabels[kind], format(result.value), true]]);
        } catch (error) { fail("explog-application-result", error); }
    }
    applicationKind.addEventListener("change", () => { applicationFields(); calculateApplication(); });
    document.getElementById("explog-application-calculate").addEventListener("click", calculateApplication);
    applicationFields();

    calculatePowers();
    calculateExponential();
    calculateLogarithm();
    calculateEquation();
    calculateApplication();

    setupExponentialLogarithmicPractice({ root: document.getElementById("explog-practice-root"), format });
}
