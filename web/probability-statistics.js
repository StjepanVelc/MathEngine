function setupProbabilityStatistics(module) {
    const toggles = [...document.querySelectorAll(".probability-statistics-toggle")];
    const visuals = window.ProbabilityStatisticsVisuals;
    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".probability-statistics-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) { panel.hidden = false; button.classList.add("active"); }
    }));

    function num(id) { const input = document.getElementById(id); const value = Number(input.value); if (input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upisi ispravan broj."); return value; }
    function int(id) { const input = document.getElementById(id); const value = Number(input.value); if (input.value.trim() === "" || !Number.isFinite(value) || !Number.isInteger(value)) throw new Error("Upisi ispravan cijeli broj."); return value; }
    function text(id) { const value = document.getElementById(id).value.trim(); if (!value) throw new Error("Upisi vrijednost."); return value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "probability-statistics-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }
    function fmt(value) { return new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value); }

    function calculateBinomial() {
        try {
            const n = int("ps-binomial-n"), p = num("ps-binomial-p"), k = int("ps-binomial-k");
            const result = parse(module.probabilityStatisticsBinomial(n, p, k), "ps-binomial-result");
            if (!result) return;
            render("ps-binomial-result", [
                ["P(X=k)", fmt(result.probability), true],
                ["Ocekivanje E(X)", fmt(result.mean)],
                ["Varijanca D(X)", fmt(result.variance)]
            ]);
            steps("ps-binomial-steps", result.steps);
        } catch (error) { fail("ps-binomial-result", error); }
    }
    document.getElementById("ps-binomial-calculate").addEventListener("click", calculateBinomial);

    function calculatePoisson() {
        try {
            const lambda = num("ps-poisson-lambda"), k = int("ps-poisson-k");
            const result = parse(module.probabilityStatisticsPoisson(lambda, k), "ps-poisson-result");
            if (!result) return;
            render("ps-poisson-result", [
                ["P(X=k)", fmt(result.probability), true],
                ["Ocekivanje E(X)", fmt(result.mean)],
                ["Varijanca D(X)", fmt(result.variance)]
            ]);
            steps("ps-poisson-steps", result.steps);
        } catch (error) { fail("ps-poisson-result", error); }
    }
    document.getElementById("ps-poisson-calculate").addEventListener("click", calculatePoisson);

    function calculateNormal() {
        try {
            const mean = num("ps-normal-mean"), stdDev = num("ps-normal-std"), x = num("ps-normal-x");
            const result = parse(module.probabilityStatisticsNormal(mean, stdDev, x), "ps-normal-result");
            if (!result) return;
            render("ps-normal-result", [
                ["Gustoca f(x)", fmt(result.density), true],
                ["Kumulativna P(X<=x)", fmt(result.cumulative)],
                ["z-vrijednost", fmt(result.zScore)]
            ]);
            steps("ps-normal-steps", result.steps);
            visuals?.normalCurve("ps-normal-visual", mean, stdDev);
        } catch (error) { fail("ps-normal-result", error); }
    }
    document.getElementById("ps-normal-calculate").addEventListener("click", calculateNormal);

    function calculateUniform() {
        try {
            const a = num("ps-uniform-a"), b = num("ps-uniform-b"), x = num("ps-uniform-x");
            const result = parse(module.probabilityStatisticsUniform(a, b, x), "ps-uniform-result");
            if (!result) return;
            render("ps-uniform-result", [
                ["Gustoca f(x)", fmt(result.density), true],
                ["Kumulativna F(x)", fmt(result.cumulative)],
                ["Ocekivanje E(X)", fmt(result.mean)],
                ["Varijanca D(X)", fmt(result.variance)]
            ]);
            steps("ps-uniform-steps", result.steps);
        } catch (error) { fail("ps-uniform-result", error); }
    }
    document.getElementById("ps-uniform-calculate").addEventListener("click", calculateUniform);

    function calculateConfidenceInterval() {
        try {
            const sample = text("ps-sample"), level = num("ps-confidence-level"), stdDev = num("ps-population-std");
            const result = parse(module.probabilityStatisticsConfidenceInterval(sample, level, stdDev), "ps-confidence-result");
            if (!result) return;
            render("ps-confidence-result", [
                ["Uzoracka sredina", fmt(result.sampleMean), true],
                ["Granica pogreske", fmt(result.marginOfError)],
                ["Interval", `(${fmt(result.lowerBound)}, ${fmt(result.upperBound)})`]
            ]);
            steps("ps-confidence-steps", result.steps);
        } catch (error) { fail("ps-confidence-result", error); }
    }
    document.getElementById("ps-confidence-calculate").addEventListener("click", calculateConfidenceInterval);

    function calculateZTest() {
        try {
            const sample = text("ps-sample"), hypothesizedMean = num("ps-ztest-mean"), stdDev = num("ps-population-std"), alpha = num("ps-ztest-alpha");
            const result = parse(module.probabilityStatisticsZTest(sample, hypothesizedMean, stdDev, alpha), "ps-ztest-result");
            if (!result) return;
            render("ps-ztest-result", [
                ["Testna statistika z", fmt(result.testStatistic), true],
                ["Kriticna vrijednost", fmt(result.criticalValue)],
                ["Odluka", result.rejectNull ? "odbacujemo H0" : "ne odbacujemo H0"]
            ]);
            steps("ps-ztest-steps", result.steps);
        } catch (error) { fail("ps-ztest-result", error); }
    }
    document.getElementById("ps-ztest-calculate").addEventListener("click", calculateZTest);

    function calculateRegression() {
        try {
            const xText = text("ps-regression-x"), yText = text("ps-regression-y");
            const result = parse(module.probabilityStatisticsRegression(xText, yText), "ps-regression-result");
            if (!result) return;
            render("ps-regression-result", [
                ["Regresijski pravac", `y = ${fmt(result.slope)}x + ${fmt(result.intercept)}`, true],
                ["Koeficijent korelacije r", fmt(result.correlation)],
                ["Koeficijent determinacije r^2", fmt(result.rSquared)]
            ]);
            steps("ps-regression-steps", result.steps);
            const xValues = xText.split(",").map((s) => Number(s.trim()));
            const yValues = yText.split(",").map((s) => Number(s.trim()));
            visuals?.regressionScatter("ps-regression-visual", xValues, yValues, result.slope, result.intercept);
        } catch (error) { fail("ps-regression-result", error); }
    }
    document.getElementById("ps-regression-calculate").addEventListener("click", calculateRegression);
}
