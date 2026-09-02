function setupCalculusBasics(module) {
    const visuals = window.CalculusBasicsVisuals || {};
    const toggles = [...document.querySelectorAll(".calculus-basics-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".calculus-basics-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); visuals.refresh?.(panel.id); } }));

    function number(id) { const input = document.getElementById(id); const value = Number(input.value); if (!input || input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj."); return value; }
    function text(id) { const input = document.getElementById(id); const value = (input.value || "").trim(); if (!value) throw new Error("Upiši izraz s varijablom x."); return value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "calculus-basics-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }

    // Limes
    function calculateLimit() {
        try {
            const expression = text("calculus-limit-expression");
            const point = number("calculus-limit-point");
            const result = parse(module.calculusLimit(expression, point, "x"), "calculus-limit-result");
            if (!result) return;
            render("calculus-limit-result", [["Limes", format(result.limitValue), true], ["Postoji konačan limes", result.existsFinite ? "da" : "ne"]]);
            steps("calculus-limit-steps", result.steps);
            visuals.limitApproach?.("calculus-limit-visual", result.point, result.limitValue, result.leftSamples, result.rightSamples);
        } catch (error) { fail("calculus-limit-result", error); }
    }
    document.getElementById("calculus-limit-calculate").addEventListener("click", calculateLimit);

    // Derivacija
    function calculateDerivative() {
        try {
            const expression = text("calculus-derivative-expression");
            const point = number("calculus-derivative-point");
            const result = parse(module.calculusDerivative(expression, point, "x"), "calculus-derivative-result");
            if (!result) return;
            render("calculus-derivative-result", [["Derivacija", result.derivative, true], ["Nagib u točki", format(result.slopeAtPoint)], ["Tangenta", result.tangentLine]]);
            steps("calculus-derivative-steps", result.steps);
        } catch (error) { fail("calculus-derivative-result", error); }
    }
    document.getElementById("calculus-derivative-calculate").addEventListener("click", calculateDerivative);

    function calculateRateOfChange() {
        try {
            const expression = text("calculus-rate-expression");
            const a = number("calculus-rate-a");
            const b = number("calculus-rate-b");
            const result = parse(module.calculusRateOfChange(expression, a, b, "x"), "calculus-rate-result");
            if (!result) return;
            render("calculus-rate-result", [["Prosječna brzina promjene", format(result.averageRate), true], ["Trenutna brzina promjene u a", format(result.instantaneousRate)]]);
            steps("calculus-rate-steps", result.steps);
        } catch (error) { fail("calculus-rate-result", error); }
    }
    document.getElementById("calculus-rate-calculate").addEventListener("click", calculateRateOfChange);

    // Primjene derivacije
    function calculateApplications() {
        try {
            const expression = text("calculus-applications-expression");
            const result = parse(module.calculusDerivativeApplications(expression, "x"), "calculus-applications-result");
            if (!result) return;
            const rows = [["Derivacija", result.derivative, true]];
            result.criticalPoints.forEach((point) => rows.push([`Kritična točka (${point.kind})`, `x=${format(point.x)}, y=${format(point.y)}`]));
            rows.push(["Rastuća na", result.increasingIntervals.join("; ") || "nema"]);
            rows.push(["Padajuća na", result.decreasingIntervals.join("; ") || "nema"]);
            render("calculus-applications-result", rows);
            steps("calculus-applications-steps", result.steps);
        } catch (error) { fail("calculus-applications-result", error); }
    }
    document.getElementById("calculus-applications-calculate").addEventListener("click", calculateApplications);

    // Odredeni integral
    function calculateIntegral() {
        try {
            const expression = text("calculus-integral-expression");
            const lowerBound = number("calculus-integral-lower");
            const upperBound = number("calculus-integral-upper");
            const result = parse(module.calculusDefiniteIntegral(expression, lowerBound, upperBound, "x"), "calculus-integral-result");
            if (!result) return;
            render("calculus-integral-result", [["Antiderivacija", result.antiderivative, true], ["Površina (integral)", format(result.area)], ["Numerička provjera (Simpson)", format(result.numericCheck)]]);
            steps("calculus-integral-steps", result.steps);
        } catch (error) { fail("calculus-integral-result", error); }
    }
    document.getElementById("calculus-integral-calculate").addEventListener("click", calculateIntegral);

    setupCalculusBasicsPractice({ root: document.getElementById("calculus-basics-practice-root"), format });
}
