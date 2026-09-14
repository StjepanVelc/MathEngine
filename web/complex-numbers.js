function setupComplexNumbers(module) {
    const toggles = [...document.querySelectorAll(".complex-numbers-toggle")];
    const visuals = window.ComplexNumbersVisuals;
    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".complex-numbers-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) { panel.hidden = false; button.classList.add("active"); }
    }));

    function num(id) { const input = document.getElementById(id); const value = Number(input.value); if (input.value.trim() === "" || !Number.isFinite(value)) throw new Error("Upisi ispravan broj."); return value; }
    function int(id) { const input = document.getElementById(id); const value = Number(input.value); if (input.value.trim() === "" || !Number.isFinite(value) || !Number.isInteger(value)) throw new Error("Upisi ispravan cijeli broj."); return value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "complex-numbers-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }
    function fmt(value) { return new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value); }
    function complexText(real, imaginary) { return `${fmt(real)} ${imaginary >= 0 ? "+" : "-"} ${fmt(Math.abs(imaginary))}i`; }

    function calculateOperation() {
        try {
            const aReal = num("cn-op-a-real"), aImag = num("cn-op-a-imag"), bReal = num("cn-op-b-real"), bImag = num("cn-op-b-imag");
            const mode = document.getElementById("cn-op-mode").value;
            const result = parse(module.complexNumbersOperation(mode, aReal, aImag, bReal, bImag), "cn-op-result");
            if (!result) return;
            render("cn-op-result", [["Rezultat", complexText(result.real, result.imaginary), true]]);
            steps("cn-op-steps", result.steps);
            visuals?.operation("cn-op-visual", { real: aReal, imaginary: aImag }, { real: bReal, imaginary: bImag }, { real: result.real, imaginary: result.imaginary });
        } catch (error) { fail("cn-op-result", error); }
    }
    document.getElementById("cn-op-calculate").addEventListener("click", calculateOperation);

    function calculateConjugate() {
        try {
            const real = num("cn-conj-real"), imaginary = num("cn-conj-imag");
            const result = parse(module.complexNumbersConjugate(real, imaginary), "cn-conj-result");
            if (!result) return;
            render("cn-conj-result", [["Konjugat", complexText(result.real, result.imaginary), true]]);
            steps("cn-conj-steps", result.steps);
            visuals?.operation("cn-conj-visual", { real, imaginary }, { real: result.real, imaginary: result.imaginary }, { real, imaginary: 0 });
        } catch (error) { fail("cn-conj-result", error); }
    }
    document.getElementById("cn-conj-calculate").addEventListener("click", calculateConjugate);

    function calculatePolarForm() {
        try {
            const real = num("cn-polar-real"), imaginary = num("cn-polar-imag");
            const result = parse(module.complexNumbersToPolarForm(real, imaginary), "cn-polar-result");
            if (!result) return;
            render("cn-polar-result", [
                ["Modul r", fmt(result.modulus), true],
                ["Argument", `${fmt(result.argumentDegrees)} stupnjeva (${fmt(result.argument)} rad)`]
            ]);
            steps("cn-polar-steps", result.steps);
            visuals?.singleValue("cn-polar-visual", { real, imaginary }, "z");
        } catch (error) { fail("cn-polar-result", error); }
    }
    document.getElementById("cn-polar-calculate").addEventListener("click", calculatePolarForm);

    function calculateAlgebraicForm() {
        try {
            const modulus = num("cn-alg-modulus"), degrees = num("cn-alg-degrees");
            const result = parse(module.complexNumbersToAlgebraicForm(modulus, degrees), "cn-alg-result");
            if (!result) return;
            render("cn-alg-result", [["Algebarski oblik", complexText(result.real, result.imaginary), true]]);
            steps("cn-alg-steps", result.steps);
        } catch (error) { fail("cn-alg-result", error); }
    }
    document.getElementById("cn-alg-calculate").addEventListener("click", calculateAlgebraicForm);

    function calculatePower() {
        try {
            const real = num("cn-power-real"), imaginary = num("cn-power-imag"), exponent = int("cn-power-exponent");
            const result = parse(module.complexNumbersPower(real, imaginary, exponent), "cn-power-result");
            if (!result) return;
            render("cn-power-result", [["Rezultat", complexText(result.real, result.imaginary), true]]);
            steps("cn-power-steps", result.steps);
        } catch (error) { fail("cn-power-result", error); }
    }
    document.getElementById("cn-power-calculate").addEventListener("click", calculatePower);

    function calculateRoots() {
        try {
            const real = num("cn-power-real"), imaginary = num("cn-power-imag"), degree = int("cn-roots-degree");
            const result = parse(module.complexNumbersNthRoots(real, imaginary, degree), "cn-roots-result");
            if (!result) return;
            render("cn-roots-result", result.roots.map((root, index) => [`w${index}`, complexText(root.real, root.imaginary), index === 0]));
            steps("cn-roots-steps", result.steps);
            visuals?.roots("cn-roots-visual", result.roots);
        } catch (error) { fail("cn-roots-result", error); }
    }
    document.getElementById("cn-roots-calculate").addEventListener("click", calculateRoots);

    function calculateQuadratic() {
        try {
            const a = num("cn-quad-a"), b = num("cn-quad-b"), c = num("cn-quad-c");
            const result = parse(module.complexNumbersSolveQuadratic(a, b, c), "cn-quad-result");
            if (!result) return;
            render("cn-quad-result", [
                ["x1", complexText(result.root1Real, result.root1Imaginary), true],
                ["x2", complexText(result.root2Real, result.root2Imaginary)]
            ]);
            steps("cn-quad-steps", result.steps);
            visuals?.quadraticRoots("cn-quad-visual", { real: result.root1Real, imaginary: result.root1Imaginary }, { real: result.root2Real, imaginary: result.root2Imaginary });
        } catch (error) { fail("cn-quad-result", error); }
    }
    document.getElementById("cn-quad-calculate").addEventListener("click", calculateQuadratic);
}
