// Aritmetička podpoglavlja pogonjena zasebnim WASM API-jima.

function setupArithmetic(module) {
    function requireValues(...inputs) {
        if (inputs.some((input) => !input.value.trim())) throw new Error("Sva polja moraju biti popunjena.");
    }

    const toggles = document.querySelectorAll(".arithmetic-toggle");
    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".arithmetic-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) {
            panel.hidden = false;
            button.classList.add("active");
        }
    }));

    const calcInput = document.getElementById("arith-input");
    const calcResult = document.getElementById("arith-result");
    setupPalette(document.querySelector('.symbol-palette[data-target="arith-input"]'));
    function calculate() {
        const text = calcInput.value.trim() ? module.evaluateArithmetic(calcInput.value.trim()) : "—";
        calcResult.textContent = text;
        calcResult.classList.toggle("error", text.startsWith("GRESKA:"));
    }
    document.getElementById("arith-eval").addEventListener("click", calculate);
    calcInput.addEventListener("keydown", (event) => { if (event.key === "Enter") calculate(); });

    document.getElementById("fraction-eval").addEventListener("click", () => {
        const leftInput = document.getElementById("fraction-left");
        const rightInput = document.getElementById("fraction-right");
        try { requireValues(leftInput, rightInput); } catch (error) {
            document.getElementById("fraction-result").textContent = "GRESKA: " + error.message;
            return;
        }
        const text = module.rationalCalculate(
            leftInput.value.trim(),
            document.getElementById("fraction-operation").value,
            rightInput.value.trim());
        const box = document.getElementById("fraction-result");
        box.innerHTML = "";
        if (text.startsWith("GRESKA:")) {
            box.textContent = text;
            box.className = "error";
            return;
        }
        box.className = "";
        const value = JSON.parse(text);
        box.textContent = `Točno: ${value.exact} | Mješovito: ${value.mixed} | Decimalno: ${value.decimal}`;
    });
    document.getElementById("decimal-fraction-eval").addEventListener("click", () => {
        const result = module.rationalFromDecimal(document.getElementById("decimal-fraction-input").value.trim());
        document.getElementById("decimal-fraction-result").textContent = result;
    });

    document.getElementById("percentage-eval").addEventListener("click", () => {
        const firstInput = document.getElementById("percentage-first");
        const secondInput = document.getElementById("percentage-second");
        try { requireValues(firstInput, secondInput); } catch (error) {
            document.getElementById("percentage-result").textContent = "GRESKA: " + error.message;
            return;
        }
        const result = module.percentageCalculate(
            document.getElementById("percentage-operation").value,
            Number(firstInput.value), Number(secondInput.value));
        const output = document.getElementById("percentage-result");
        output.textContent = result;
        output.classList.toggle("error", result.startsWith("GRESKA:"));
    });

    document.getElementById("number-theory-eval").addEventListener("click", () => {
        const numberInput = document.getElementById("number-theory-input");
        if (!numberInput.value.trim()) {
            document.getElementById("number-theory-result").textContent = "GRESKA: Upiši prirodan broj.";
            return;
        }
        const text = module.numberTheoryAnalyze(numberInput.value.trim());
        const box = document.getElementById("number-theory-result");
        if (text.startsWith("GRESKA:")) {
            box.textContent = text;
            box.className = "error";
            return;
        }
        box.className = "";
        const value = JSON.parse(text);
        const factors = value.factors.length
            ? value.factors.map((factor) => `${factor.prime}${factor.exponent > 1 ? `^${factor.exponent}` : ""}`).join(" × ")
            : "—";
        box.textContent = `Prost: ${value.prime ? "da" : "ne"} | Djelitelji: ${value.divisors.join(", ")} | Faktorizacija: ${factors}`;
    });
    document.getElementById("gcd-eval").addEventListener("click", () => {
        const text = module.numberTheoryGcdLcm(
            document.getElementById("gcd-left").value.trim(),
            document.getElementById("gcd-right").value.trim());
        const output = document.getElementById("gcd-result");
        if (text.startsWith("GRESKA:")) {
            output.textContent = text;
            output.classList.add("error");
            return;
        }
        const value = JSON.parse(text);
        output.textContent = `NZD: ${value.gcd}, NZV: ${value.lcm}`;
        output.classList.remove("error");
    });

    document.getElementById("numeral-eval").addEventListener("click", () => {
        const numeralInput = document.getElementById("numeral-input");
        if (!numeralInput.value.trim()) {
            document.getElementById("numeral-result").textContent = "GRESKA: Upiši broj za pretvorbu.";
            return;
        }
        const result = module.convertNumeralSystem(
            numeralInput.value.trim(),
            Number(document.getElementById("numeral-from").value),
            Number(document.getElementById("numeral-to").value));
        const output = document.getElementById("numeral-result");
        output.textContent = result;
        output.classList.toggle("error", result.startsWith("GRESKA:"));
    });
}
