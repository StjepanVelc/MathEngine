function setupSequences(module) {
    const visuals = window.SequencesVisuals || {};
    const toggles = [...document.querySelectorAll(".sequences-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);

    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".sequences-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) { panel.hidden = false; button.classList.add("active"); visuals.refresh?.(panel.id); }
    }));

    function number(id) {
        const input = document.getElementById(id);
        const value = Number(input.value);
        if (input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj.");
        return value;
    }
    function integer(id, maximum = 10000) {
        const value = number(id);
        if (!Number.isInteger(value) || value < 1 || value > maximum) throw new Error(`Upiši cijeli broj od 1 do ${maximum}.`);
        return value;
    }
    function parse(text, boxId) {
        if (text.startsWith("GRESKA:")) { fail(boxId, text); return null; }
        return JSON.parse(text);
    }
    function fail(boxId, error) {
        const box = document.getElementById(boxId); box.innerHTML = "";
        box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error");
    }
    function render(boxId, rows) {
        const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error");
        rows.forEach(([label, value, primary = false]) => {
            const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`;
            const name = document.createElement("span"); name.textContent = label;
            const content = document.createElement("strong"); content.textContent = value;
            row.append(name, content); box.append(row);
        });
    }
    function steps(boxId, items) {
        const box = document.getElementById(boxId); box.innerHTML = "";
        items.forEach((text, index) => {
            const item = document.createElement("div"); item.className = "sequences-step";
            const badge = document.createElement("span"); badge.textContent = index + 1;
            const content = document.createElement("p"); content.textContent = text;
            item.append(badge, content); box.append(item);
        });
    }
    function table(containerId, terms) {
        const shell = document.getElementById(containerId); shell.innerHTML = "";
        const tableElement = document.createElement("table");
        const indexes = document.createElement("tr"); const values = document.createElement("tr");
        const indexTitle = document.createElement("th"); indexTitle.textContent = "n"; indexes.append(indexTitle);
        const valueTitle = document.createElement("th"); valueTitle.textContent = "aₙ"; values.append(valueTitle);
        terms.forEach((term, index) => {
            const indexCell = document.createElement("td"); indexCell.textContent = index + 1;
            const valueCell = document.createElement("td"); valueCell.textContent = format(term);
            indexes.append(indexCell); values.append(valueCell);
        });
        tableElement.append(indexes, values); shell.append(tableElement);
    }
    const list = (terms) => terms.slice(0, 10).map(format).join(", ") + (terms.length > 10 ? ", …" : "");

    const monotonicityLabels = {
        strictlyIncreasing: "strogo rastući", nonDecreasing: "nepadajući", constant: "konstantan",
        nonIncreasing: "nerastući", strictlyDecreasing: "strogo padajući", notMonotonic: "nije monoton"
    };
    function analyzeIntroduction() {
        const result = parse(module.sequenceAnalyze(document.getElementById("sequences-intro-terms").value), "sequences-intro-result");
        if (!result) return;
        render("sequences-intro-result", [["Vrsta niza", monotonicityLabels[result.monotonicity], true], ["Najmanji član", format(result.minimum)], ["Najveći član", format(result.maximum)], ["Razlike", result.differences.map(format).join(", ") || "—"]]);
        table("sequences-intro-table", result.terms); visuals.points?.("sequences-intro-visual", result.terms, { title: "Članovi aₙ" });
    }
    document.getElementById("sequences-intro-analyze").addEventListener("click", () => { try { analyzeIntroduction(); } catch (error) { fail("sequences-intro-result", error); } });

    function calculateArithmetic() {
        try {
            const first = number("sequences-arithmetic-first"), difference = number("sequences-arithmetic-difference");
            const count = integer("sequences-arithmetic-count", 100), searched = number("sequences-arithmetic-search");
            const result = parse(module.sequenceArithmetic(first, difference, count, searched), "sequences-arithmetic-result"); if (!result) return;
            render("sequences-arithmetic-result", [[`a${count}`, format(result.lastTerm), true], [`S${count}`, format(result.sum)], ["Traženi član", result.foundIndex ? `a${result.foundIndex}` : "nije u nizu"], ["Prvih članova", list(result.terms)]]);
            steps("sequences-arithmetic-steps", [`Prepoznaj a₁ = ${format(first)} i stalnu razliku d = ${format(difference)}.`, `Uvrsti u aₙ = a₁ + (n − 1)d: a${count} = ${format(result.lastTerm)}.`, `Zbroji prvih ${count} članova formulom Sₙ = n(a₁ + aₙ)/2.`]);
            visuals.combined?.("sequences-arithmetic-visual", result.terms, result.partialSums, { title: "Aritmetički rast" });
        } catch (error) { fail("sequences-arithmetic-result", error); }
    }
    document.getElementById("sequences-arithmetic-calculate").addEventListener("click", calculateArithmetic);

    function calculateGeometric() {
        try {
            const first = number("sequences-geometric-first"), ratio = number("sequences-geometric-ratio");
            const count = integer("sequences-geometric-count", 100), searched = number("sequences-geometric-search");
            const result = parse(module.sequenceGeometric(first, ratio, count, searched), "sequences-geometric-result"); if (!result) return;
            render("sequences-geometric-result", [[`a${count}`, format(result.lastTerm), true], [`S${count}`, format(result.sum)], ["Beskonačni red", result.converges ? `konvergira prema ${format(result.infiniteSum)}` : "divergira"], ["Traženi član", result.foundIndex ? `a${result.foundIndex}` : "nije pronađen"]]);
            steps("sequences-geometric-steps", [`Prepoznaj a₁ = ${format(first)} i stalni kvocijent q = ${format(ratio)}.`, `Primijeni aₙ = a₁qⁿ⁻¹ za ${count}. član.`, result.converges ? "Budući da je |q| < 1, parcijalne sume približavaju se konačnoj granici." : "Budući da |q| nije manji od 1, beskonačni red nema konačnu sumu."]);
            visuals.combined?.("sequences-geometric-visual", result.terms, result.partialSums, { title: "Geometrijski rast", limit: result.infiniteSum });
        } catch (error) { fail("sequences-geometric-result", error); }
    }
    document.getElementById("sequences-geometric-calculate").addEventListener("click", calculateGeometric);

    const recurrenceType = document.getElementById("sequences-recurrence-type");
    function updateRecurrenceLabels() {
        document.getElementById("sequences-recurrence-first-label").firstChild.textContent = "Prvi član";
        document.getElementById("sequences-recurrence-second-label").firstChild.textContent = recurrenceType.value === "fibonacci" ? "Drugi član" : recurrenceType.value === "arithmetic" ? "Diferencija d" : "Kvocijent q";
    }
    function calculateRecurrence() {
        try {
            const result = parse(module.sequenceRecurrence(recurrenceType.value, number("sequences-recurrence-first"), number("sequences-recurrence-second"), integer("sequences-recurrence-count", 50)), "sequences-recurrence-result"); if (!result) return;
            render("sequences-recurrence-result", [["Razvijeni niz", list(result.terms), true], ["Broj članova", String(result.terms.length)], ["Eksplicitni i rekurzivni zapis", recurrenceType.value === "fibonacci" ? "rekurzivni model" : result.equivalent ? "daju iste članove" : "razlikuju se"]]);
            const rule = recurrenceType.value === "fibonacci" ? "Zbroji prethodna dva člana." : recurrenceType.value === "arithmetic" ? "Prethodnom članu dodaj d." : "Prethodni član pomnoži s q.";
            steps("sequences-recurrence-steps", ["Zapiši početni član ili početna dva člana.", rule, "Ponavljaj pravilo dok ne dobiješ traženi broj članova."]);
            visuals.recurrence?.("sequences-recurrence-visual", result.terms, recurrenceType.value);
        } catch (error) { fail("sequences-recurrence-result", error); }
    }
    recurrenceType.addEventListener("change", updateRecurrenceLabels);
    document.getElementById("sequences-recurrence-calculate").addEventListener("click", calculateRecurrence);

    const applicationType = document.getElementById("sequences-application-type");
    function updateApplicationFields() {
        const type = applicationType.value;
        document.getElementById("sequences-application-frequency-label").hidden = !["compoundInterest", "amortization"].includes(type);
        document.getElementById("sequences-application-principal-label").firstChild.textContent = type === "population" ? "Početna populacija" : type === "amortization" ? "Iznos zajma" : "Početna vrijednost";
    }
    function calculateApplication() {
        try {
            const type = applicationType.value;
            const result = parse(module.sequenceApplication(type, number("sequences-application-principal"), number("sequences-application-rate"), integer("sequences-application-periods", 100), integer("sequences-application-frequency", 365)), "sequences-application-result"); if (!result) return;
            if (type === "amortization") render("sequences-application-result", [["Periodična rata", format(result.periodicPayment), true], ["Ukupno plaćeno", format(result.totalPaid)], ["Ukupna kamata", format(result.totalInterest)], ["Broj rata", String(result.values.length - 1)]]);
            else render("sequences-application-result", [["Završna vrijednost", format(result.finalValue), true], ["Ukupna promjena", format(result.totalChange)], ["Početna vrijednost", format(result.initialValue)]]);
            const model = { simpleInterest: "Svake godine dodaje se isti iznos kamate na početnu glavnicu.", compoundInterest: "Svaki obračun množi trenutačni iznos faktorom rasta.", population: "Svako razdoblje nova populacija postaje baza sljedećeg rasta.", percentage: "Isti postotak primjenjuje se uzastopno, zato promjene nisu linearne.", amortization: "Svaka rata prvo pokriva kamatu, a ostatak smanjuje glavnicu." }[type];
            steps("sequences-application-steps", [model, "Izračunaj vrijednost nakon svakog razdoblja.", "Usporedi početnu i konačnu vrijednost te protumači promjenu."]);
            visuals.application?.("sequences-application-visual", result.values, type);
        } catch (error) { fail("sequences-application-result", error); }
    }
    applicationType.addEventListener("change", updateApplicationFields);
    document.getElementById("sequences-application-calculate").addEventListener("click", calculateApplication);

    updateRecurrenceLabels(); updateApplicationFields(); analyzeIntroduction(); calculateArithmetic(); calculateGeometric(); calculateRecurrence(); calculateApplication();
    window.setupSequencesPractice?.({ root: document.getElementById("sequences-practice-root"), format });
}
