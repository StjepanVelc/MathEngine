function setupMathematicalAnalysis(module) {
    const visuals = window.MathematicalAnalysisVisuals || {};
    const toggles = [...document.querySelectorAll(".mathematical-analysis-toggle")];
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".mathematical-analysis-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); visuals.refresh?.(panel.id); } }));

    function number(id) { const input = document.getElementById(id); const value = Number(input.value); if (!input || input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj."); return value; }
    function text(id) { const input = document.getElementById(id); const value = (input.value || "").trim(); if (!value) throw new Error("Upiši izraz s varijablom x."); return value; }
    function select(id) { return document.getElementById(id).value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "mathematical-analysis-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }

    // Formalni limes
    function calculateFormalLimit() {
        try {
            const expression = text("ma-limit-expression");
            const point = number("ma-limit-point");
            const result = parse(module.mathematicalAnalysisFormalLimit(expression, point, "x"), "ma-limit-result");
            if (!result) return;
            render("ma-limit-result", [["Limes", format(result.limitValue), true], ["Postoji konačan limes", result.existsFinite ? "da" : "ne"]]);
            steps("ma-limit-steps", result.steps);
            const table = document.getElementById("ma-limit-table"); table.innerHTML = "";
            const heading = document.createElement("p"); heading.textContent = "ε-δ tablica"; table.append(heading);
            result.epsilonDeltaTable.forEach((row) => { const line = document.createElement("div"); line.className = "mathematical-analysis-step"; const content = document.createElement("p"); content.textContent = `ε = ${format(row.epsilon)} ⟹ δ = ${format(row.delta)}`; line.append(content); table.append(line); });
        } catch (error) { fail("ma-limit-result", error); }
    }
    document.getElementById("ma-limit-calculate").addEventListener("click", calculateFormalLimit);

    function calculateContinuity() {
        try {
            const expression = text("ma-continuity-expression");
            const point = number("ma-continuity-point");
            const result = parse(module.mathematicalAnalysisContinuity(expression, point, "x"), "ma-continuity-result");
            if (!result) return;
            render("ma-continuity-result", [["Neprekidna u točki", result.isContinuous ? "da" : "ne", true], ["f(točka)", format(result.functionValueAtPoint)], ["Limes u točki", format(result.limitValue)]]);
            steps("ma-continuity-steps", result.steps);
        } catch (error) { fail("ma-continuity-result", error); }
    }
    document.getElementById("ma-continuity-calculate").addEventListener("click", calculateContinuity);

    // Napredne derivacije
    function calculateNthDerivative() {
        try {
            const expression = text("ma-nth-expression");
            const order = number("ma-nth-order");
            const point = number("ma-nth-point");
            const result = parse(module.mathematicalAnalysisNthDerivative(expression, order, point, "x"), "ma-nth-result");
            if (!result) return;
            render("ma-nth-result", [["n-ta derivacija", result.derivativeExpression, true], ["Vrijednost u točki", format(result.derivativeValueAtPoint)]]);
            steps("ma-nth-steps", result.steps);
        } catch (error) { fail("ma-nth-result", error); }
    }
    document.getElementById("ma-nth-calculate").addEventListener("click", calculateNthDerivative);

    function calculateChainRule() {
        try {
            const outer = text("ma-chain-outer");
            const inner = text("ma-chain-inner");
            const point = number("ma-chain-point");
            const result = parse(module.mathematicalAnalysisChainRule(outer, inner, point, "x"), "ma-chain-result");
            if (!result) return;
            render("ma-chain-result", [["(f∘g)'(x)", format(result.compositeDerivativeAtPoint), true], ["g(x)", format(result.innerValueAtPoint)], ["f'(g(x))", format(result.outerDerivativeAtInnerValue)], ["g'(x)", format(result.innerDerivativeAtPoint)]]);
            steps("ma-chain-steps", result.steps);
        } catch (error) { fail("ma-chain-result", error); }
    }
    document.getElementById("ma-chain-calculate").addEventListener("click", calculateChainRule);

    // Napredni integrali
    function calculateImproperIntegral() {
        try {
            const functionName = select("ma-improper-function");
            const lowerBound = number("ma-improper-lower");
            const result = parse(module.mathematicalAnalysisImproperIntegral(functionName, lowerBound), "ma-improper-result");
            if (!result) return;
            render("ma-improper-result", [["Konvergira", result.convergent ? "da" : "ne", true], ["Približna vrijednost", format(result.approximateValue)]]);
            steps("ma-improper-steps", result.steps);
        } catch (error) { fail("ma-improper-result", error); }
    }
    document.getElementById("ma-improper-calculate").addEventListener("click", calculateImproperIntegral);

    function calculateSubstitutionIntegral() {
        try {
            const a = number("ma-subst-a");
            const b = number("ma-subst-b");
            const power = number("ma-subst-power");
            const lowerBound = number("ma-subst-lower");
            const upperBound = number("ma-subst-upper");
            const result = parse(module.mathematicalAnalysisSubstitutionIntegral(a, b, power, lowerBound, upperBound), "ma-subst-result");
            if (!result) return;
            render("ma-subst-result", [["Vrijednost integrala", format(result.value), true]]);
            steps("ma-subst-steps", result.steps);
        } catch (error) { fail("ma-subst-result", error); }
    }
    document.getElementById("ma-subst-calculate").addEventListener("click", calculateSubstitutionIntegral);

    // Redovi funkcija
    const taylorActual = { exp: Math.exp, sin: Math.sin };
    function calculateTaylor() {
        try {
            const functionName = select("ma-taylor-function");
            const center = number("ma-taylor-center");
            const order = number("ma-taylor-order");
            const point = number("ma-taylor-point");
            const result = parse(module.mathematicalAnalysisTaylorSeries(functionName, center, order, point), "ma-taylor-result");
            if (!result) return;
            render("ma-taylor-result", [["Taylorova aproksimacija", format(result.taylorApproximationAtPoint), true], ["Stvarna vrijednost", format(result.actualValueAtPoint)], ["Pogreška", format(result.approximationError)]]);
            steps("ma-taylor-steps", result.steps);
            const actualFn = taylorActual[functionName] || Math.exp;
            const taylorFn = (x) => result.coefficients.reduce((sum, coeff, k) => sum + coeff * Math.pow(x - center, k), 0);
            visuals.taylorApproximation?.("ma-taylor-visual", center, taylorFn, actualFn, point);
        } catch (error) { fail("ma-taylor-result", error); }
    }
    document.getElementById("ma-taylor-calculate").addEventListener("click", calculateTaylor);

    function calculatePowerSeries() {
        try {
            const coefficientsText = text("ma-series-coefficients");
            const point = number("ma-series-point");
            const result = parse(module.mathematicalAnalysisPowerSeries(coefficientsText, point), "ma-series-result");
            if (!result) return;
            render("ma-series-result", [["Radijus konvergencije", format(result.radiusOfConvergence), true], ["Konvergira u testnoj točki", result.convergesAtPoint ? "da" : "ne"]]);
            steps("ma-series-steps", result.steps);
        } catch (error) { fail("ma-series-result", error); }
    }
    document.getElementById("ma-series-calculate").addEventListener("click", calculatePowerSeries);

    // Funkcije vise varijabli
    function calculatePartialDerivatives() {
        try {
            const functionName = select("ma-partial-function");
            const x = number("ma-partial-x");
            const y = number("ma-partial-y");
            const result = parse(module.mathematicalAnalysisPartialDerivatives(functionName, x, y), "ma-partial-result");
            if (!result) return;
            render("ma-partial-result", [["∂f/∂x", format(result.partialX), true], ["∂f/∂y", format(result.partialY)], ["Gradijent", `(${format(result.gradient[0])}, ${format(result.gradient[1])})`], ["|∇f|", format(result.gradientMagnitude)]]);
            steps("ma-partial-steps", result.steps);
        } catch (error) { fail("ma-partial-result", error); }
    }
    document.getElementById("ma-partial-calculate").addEventListener("click", calculatePartialDerivatives);

    function calculateDirectionalDerivative() {
        try {
            const functionName = select("ma-partial-function");
            const x = number("ma-partial-x");
            const y = number("ma-partial-y");
            const directionX = number("ma-directional-dx");
            const directionY = number("ma-directional-dy");
            const result = parse(module.mathematicalAnalysisDirectionalDerivative(functionName, x, y, directionX, directionY), "ma-directional-result");
            if (!result) return;
            render("ma-directional-result", [["Usmjerena derivacija", format(result.directionalDerivative), true], ["∂f/∂x", format(result.partialX)], ["∂f/∂y", format(result.partialY)]]);
            steps("ma-directional-steps", result.steps);
        } catch (error) { fail("ma-directional-result", error); }
    }
    document.getElementById("ma-directional-calculate").addEventListener("click", calculateDirectionalDerivative);

    // Diferencijalne jednadzbe
    function calculateOde() {
        try {
            const functionName = select("ma-ode-function");
            const initialT = number("ma-ode-t0");
            const initialY = number("ma-ode-y0");
            const finalT = number("ma-ode-tfinal");
            const stepSize = number("ma-ode-step");
            const euler = parse(module.mathematicalAnalysisSolveOde(functionName, "euler", initialT, initialY, finalT, stepSize), "ma-ode-result");
            const rk4 = parse(module.mathematicalAnalysisSolveOde(functionName, "rk4", initialT, initialY, finalT, stepSize), "ma-ode-result");
            if (!euler || !rk4) return;
            render("ma-ode-result", [["Euler y(final)", format(euler.finalValue), true], ["RK4 y(final)", format(rk4.finalValue)]]);
            steps("ma-ode-steps", rk4.steps);
            visuals.odeTrajectory?.("ma-ode-visual", euler.points, rk4.points);
        } catch (error) { fail("ma-ode-result", error); }
    }
    document.getElementById("ma-ode-calculate").addEventListener("click", calculateOde);

    setupMathematicalAnalysisPractice({ root: document.getElementById("ma-practice-root"), format });
}
