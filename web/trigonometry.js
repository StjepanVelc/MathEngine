function setupTrigonometry(module) {
    const visuals = window.TrigonometryVisuals || {};
    const toggles = [...document.querySelectorAll(".trigonometry-toggle")];

    toggles.forEach((button) => button.addEventListener("click", () => {
        const panel = document.getElementById(button.dataset.panel);
        const open = panel.hidden;
        document.querySelectorAll(".trigonometry-panel").forEach((item) => (item.hidden = true));
        toggles.forEach((item) => item.classList.remove("active"));
        if (open) {
            panel.hidden = false;
            button.classList.add("active");
            visuals.refresh?.(panel.id);
        }
    }));

    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    function number(id) {
        const input = document.getElementById(id);
        if (!input || input.value.trim() === "") throw new Error("Popuni sva potrebna polja.");
        const value = Number(input.value);
        if (!Number.isFinite(value)) throw new Error("Upiši ispravan konačan broj.");
        return value;
    }
    function parse(text, boxId) {
        if (text.startsWith("GRESKA:")) { fail(boxId, text); return null; }
        return JSON.parse(text);
    }
    function render(boxId, rows) {
        const box = document.getElementById(boxId);
        box.innerHTML = "";
        box.classList.remove("error");
        rows.forEach(([label, value, primary = false]) => {
            const row = document.createElement("div");
            row.className = `result-row${primary ? " result-primary" : ""}`;
            const name = document.createElement("span"); name.textContent = label;
            const content = document.createElement("strong"); content.textContent = value;
            row.append(name, content); box.append(row);
        });
    }
    function fail(boxId, message) {
        const box = document.getElementById(boxId);
        box.innerHTML = "";
        box.textContent = message.startsWith("GRESKA:") ? message : `GRESKA: ${message}`;
        box.classList.add("error");
    }
    function steps(boxId, items) {
        const box = document.getElementById(boxId);
        box.innerHTML = "";
        items.forEach((text, index) => {
            const item = document.createElement("div");
            item.className = "trigonometry-step";
            const badge = document.createElement("span"); badge.textContent = index + 1;
            const content = document.createElement("p"); content.textContent = text;
            item.append(badge, content); box.append(item);
        });
    }
    function createFields(containerId, definitions, prefix) {
        const container = document.getElementById(containerId);
        container.innerHTML = "";
        definitions.forEach((definition) => {
            const [key, label, value, type = "number", options = []] = definition;
            const wrapper = document.createElement("label"); wrapper.textContent = label;
            let input;
            if (type === "select") {
                input = document.createElement("select");
                options.forEach(([text, optionValue]) => input.add(new Option(text, optionValue)));
                input.value = value;
            } else {
                input = document.createElement("input"); input.type = "number"; input.step = "any"; input.value = value;
            }
            input.id = `${prefix}-${key}`;
            input.addEventListener("input", () => visuals.refresh?.(container.closest(".trigonometry-panel").id));
            wrapper.append(input); container.append(wrapper);
        });
    }

    const angle = document.getElementById("trigonometry-angle");
    const angleNumber = document.getElementById("trigonometry-angle-number");
    const angleUnit = document.getElementById("trigonometry-angle-unit");
    const angleOutput = document.getElementById("trigonometry-angle-value");
    function syncAngle(source) {
        if (source === angle) angleNumber.value = angle.value;
        else if (angleUnit.value === "degrees") angle.value = Math.max(-360, Math.min(720, Number(angleNumber.value) || 0));
        angleOutput.textContent = `${angleNumber.value}${angleUnit.value === "degrees" ? "°" : " rad"}`;
        analyzeAngle(false);
    }
    function analyzeAngle(renderResult = true) {
        const value = Number(angleNumber.value);
        if (!Number.isFinite(value)) return;
        const result = parse(module.trigonometryAngle(value, angleUnit.value), "trigonometry-angle-result");
        if (!result) return;
        if (renderResult) render("trigonometry-angle-result", [
            ["Normalizirani kut", `${format(result.degrees)}°`, true],
            ["Radijani", `${format(result.radians)} rad`],
            ["Točka T", `(${format(result.x)}, ${format(result.y)})`],
            ["Položaj", result.quadrant ? `${result.quadrant}. kvadrant` : "na koordinatnoj osi"],
            ["Referentni kut", `${format(result.referenceDegrees)}°`]
        ]);
        visuals.circle?.(result);
    }
    angle.addEventListener("input", () => syncAngle(angle));
    angleNumber.addEventListener("input", () => syncAngle(angleNumber));
    angleUnit.addEventListener("change", () => {
        const current = Number(angleNumber.value) || 0;
        angleNumber.value = angleUnit.value === "radians" ? current * Math.PI / 180 : current * 180 / Math.PI;
        angle.hidden = angleUnit.value === "radians";
        syncAngle(angleNumber);
    });
    document.getElementById("trigonometry-angle-analyze").addEventListener("click", () => analyzeAngle(true));
    analyzeAngle(true);

    const functionSelect = document.getElementById("trigonometry-function");
    const functionMode = document.getElementById("trigonometry-function-mode");
    const functionUnit = document.getElementById("trigonometry-function-unit");
    const functionInput = document.getElementById("trigonometry-function-input");
    function calculateFunction() {
        try {
            const inverse = functionMode.value === "inverse";
            const result = parse(module.trigonometryFunction(functionSelect.value, number("trigonometry-function-input"), functionUnit.value, inverse), "trigonometry-function-result");
            if (!result) return;
            if (inverse) render("trigonometry-function-result", [["Glavna vrijednost kuta", `${format(result.value)}${functionUnit.value === "degrees" ? "°" : " rad"}`, true], ["Važno", "Jednadžba može imati i periodična rješenja."]]);
            else render("trigonometry-function-result", [[`${functionSelect.value} α`, format(result.value), true], ["Točan zapis", result.hasExactForm ? result.exact.replaceAll("sqrt", "√") : "nema jednostavnog školskog zapisa"]]);
            const angleDegrees = inverse ? (functionUnit.value === "degrees" ? result.value : result.value * 180 / Math.PI) : (functionUnit.value === "degrees" ? Number(functionInput.value) : Number(functionInput.value) * 180 / Math.PI);
            visuals.wave?.(functionSelect.value, angleDegrees, result.value);
        } catch (error) { fail("trigonometry-function-result", error.message); }
    }
    [functionSelect, functionMode, functionUnit].forEach((input) => input.addEventListener("change", calculateFunction));
    functionInput.addEventListener("input", () => { if (functionMode.value === "direct") calculateFunction(); });
    document.getElementById("trigonometry-function-calculate").addEventListener("click", calculateFunction);
    calculateFunction();

    const rightDefinitions = {
        legs: [["first", "Nasuprotna kateta a", 3], ["second", "Priležeća kateta b", 4]],
        legHypotenuse: [["first", "Poznata kateta", 5], ["second", "Hipotenuza c", 13], ["side", "Vrsta poznate katete", "opposite", "select", [["Nasuprotna", "opposite"], ["Priležeća", "adjacent"]]]],
        sideAngle: [["first", "Poznata stranica", 10], ["second", "Kut α u stupnjevima", 30], ["side", "Koja je stranica poznata?", "hypotenuse", "select", [["Nasuprotna kateta", "opposite"], ["Priležeća kateta", "adjacent"], ["Hipotenuza", "hypotenuse"]]]]
    };
    const rightMode = document.getElementById("trigonometry-right-mode");
    function updateRight() { createFields("trigonometry-right-inputs", rightDefinitions[rightMode.value], "trigonometry-right"); visuals.rightTriangle?.(); }
    rightMode.addEventListener("change", updateRight);
    document.getElementById("trigonometry-right-calculate").addEventListener("click", () => {
        try {
            const side = document.getElementById("trigonometry-right-side")?.value || "";
            const result = parse(module.trigonometryRightTriangle(rightMode.value, number("trigonometry-right-first"), number("trigonometry-right-second"), side), "trigonometry-right-result");
            if (!result) return;
            render("trigonometry-right-result", [["Nasuprotna kateta", format(result.opposite), true], ["Priležeća kateta", format(result.adjacent)], ["Hipotenuza", format(result.hypotenuse)], ["Kut α", `${format(result.angleDegrees)}°`], ["Kut β", `${format(result.complementaryAngleDegrees)}°`]]);
            steps("trigonometry-right-steps", result.steps);
            visuals.rightTriangle?.(result);
        } catch (error) { fail("trigonometry-right-result", error.message); }
    });
    updateRight();

    const generalDefinitions = {
        sss: [["first", "Stranica a", 3], ["second", "Stranica b", 4], ["third", "Stranica c", 5]],
        sas: [["first", "Stranica a", 5], ["second", "Stranica b", 7], ["third", "Kut γ između njih", 60]],
        aas: [["first", "Stranica a", 10], ["second", "Kut α", 30], ["third", "Kut β", 60]]
    };
    const generalMode = document.getElementById("trigonometry-general-mode");
    function updateGeneral() { createFields("trigonometry-general-inputs", generalDefinitions[generalMode.value], "trigonometry-general"); visuals.generalTriangle?.(); }
    generalMode.addEventListener("change", updateGeneral);
    document.getElementById("trigonometry-general-calculate").addEventListener("click", () => {
        try {
            const result = parse(module.trigonometryGeneralTriangle(generalMode.value, number("trigonometry-general-first"), number("trigonometry-general-second"), number("trigonometry-general-third")), "trigonometry-general-result");
            if (!result) return;
            render("trigonometry-general-result", [["Stranice", `a=${format(result.sideA)}, b=${format(result.sideB)}, c=${format(result.sideC)}`, true], ["Kutovi", `α=${format(result.angleA)}°, β=${format(result.angleB)}°, γ=${format(result.angleC)}°`], ["Površina", format(result.area)]]);
            steps("trigonometry-general-steps", result.steps);
            visuals.generalTriangle?.(result);
        } catch (error) { fail("trigonometry-general-result", error.message); }
    });
    updateGeneral();

    const identityForm = document.getElementById("trigonometry-identity-form");
    const equationForm = document.getElementById("trigonometry-equation-form");
    function showIdentity(identity) {
        identityForm.hidden = !identity; equationForm.hidden = identity;
        document.getElementById("trigonometry-show-identity").classList.toggle("active", identity);
        document.getElementById("trigonometry-show-equation").classList.toggle("active", !identity);
    }
    document.getElementById("trigonometry-show-identity").addEventListener("click", () => showIdentity(true));
    document.getElementById("trigonometry-show-equation").addEventListener("click", () => showIdentity(false));
    document.getElementById("trigonometry-identity-check").addEventListener("click", () => {
        try {
            const result = parse(module.trigonometryIdentity(document.getElementById("trigonometry-identity").value, number("trigonometry-identity-angle")), "trigonometry-identity-result");
            if (!result) return;
            render("trigonometry-identity-result", [["Identitet", result.formula, true], ["Lijeva strana", format(result.left)], ["Desna strana", format(result.right)], ["Zaključak", result.holds ? "Vrijedi za odabrani kut" : "Ne vrijedi"]]);
            visuals.identity?.(result);
        } catch (error) { fail("trigonometry-identity-result", error.message); }
    });
    document.getElementById("trigonometry-equation-solve").addEventListener("click", () => {
        try {
            const fn = document.getElementById("trigonometry-equation-function").value;
            const result = parse(module.trigonometryEquation(fn, number("trigonometry-equation-value"), number("trigonometry-equation-min"), number("trigonometry-equation-max")), "trigonometry-identity-result");
            if (!result) return;
            render("trigonometry-identity-result", [["Rješenja", result.solutions.length ? result.solutions.map((x) => `${format(x)}°`).join(", ") : "Nema rješenja na zadanom intervalu", true], ["Periodičnost", fn === "tan" ? "180°" : "360°"]]);
            visuals.equation?.(fn, Number(document.getElementById("trigonometry-equation-value").value), result.solutions);
        } catch (error) { fail("trigonometry-identity-result", error.message); }
    });

    if (typeof setupTrigonometryPractice === "function") setupTrigonometryPractice(module, { format });
}
