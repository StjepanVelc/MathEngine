function setupDiscreteMath(module) {
    const toggles = [...document.querySelectorAll(".discrete-math-toggle")];
    const visuals = window.DiscreteMathVisuals;
    const format = (value) => new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(value);
    toggles.forEach((button) => button.addEventListener("click", () => { const panel = document.getElementById(button.dataset.panel); const open = panel.hidden; document.querySelectorAll(".discrete-math-panel").forEach((item) => (item.hidden = true)); toggles.forEach((item) => item.classList.remove("active")); if (open) { panel.hidden = false; button.classList.add("active"); visuals?.refresh?.(panel.id); } }));

    function text(id) { const value = document.getElementById(id).value.trim(); if (!value) throw new Error("Upiši vrijednost."); return value; }
    function integer(id) { const input = document.getElementById(id); const value = Number(input.value); if (input.value.trim() === "" || !Number.isFinite(value) || !Number.isInteger(value)) throw new Error("Upiši ispravan cijeli broj."); return value; }
    function parse(result, boxId) { if (result.startsWith("GRESKA:")) { fail(boxId, result); return null; } return JSON.parse(result); }
    function fail(boxId, error) { const box = document.getElementById(boxId); box.innerHTML = ""; box.textContent = String(error.message || error).replace(/^GRESKA:\s*/, "GRESKA: "); box.classList.add("error"); }
    function render(boxId, rows) { const box = document.getElementById(boxId); box.innerHTML = ""; box.classList.remove("error"); rows.forEach(([label, value, primary = false]) => { const row = document.createElement("div"); row.className = `result-row${primary ? " result-primary" : ""}`; const name = document.createElement("span"); name.textContent = label; const content = document.createElement("strong"); content.textContent = value; row.append(name, content); box.append(row); }); }
    function steps(boxId, items) { const box = document.getElementById(boxId); box.innerHTML = ""; items.forEach((entry, index) => { const item = document.createElement("div"); item.className = "discrete-math-step"; const badge = document.createElement("span"); badge.textContent = index + 1; const content = document.createElement("p"); content.textContent = entry; item.append(badge, content); box.append(item); }); }

    // Skupovi i relacije
    function calculateSetOperation() {
        try {
            const a = text("dm-set-a"), b = text("dm-set-b"), mode = document.getElementById("dm-set-mode").value;
            const result = parse(module.discreteMathSetOperation(mode, a, b), "dm-set-result");
            if (!result) return;
            render("dm-set-result", [["Rezultat", `{${result.result.join(", ")}}`, true]]);
            steps("dm-set-steps", result.steps);
            visuals?.setOperation("dm-set-visual", a, b, mode);
        } catch (error) { fail("dm-set-result", error); }
    }
    document.getElementById("dm-set-calculate").addEventListener("click", calculateSetOperation);

    function calculateRelation() {
        try {
            const domain = text("dm-relation-domain"), pairs = text("dm-relation-pairs");
            const result = parse(module.discreteMathRelationProperties(domain, pairs), "dm-relation-result");
            if (!result) return;
            render("dm-relation-result", [
                ["Refleksivna", result.reflexive ? "da" : "ne"],
                ["Simetricna", result.symmetric ? "da" : "ne"],
                ["Antisimetricna", result.antisymmetric ? "da" : "ne"],
                ["Tranzitivna", result.transitive ? "da" : "ne"],
                ["Relacija ekvivalencije", result.isEquivalence ? "da" : "ne", true],
                ["Parcijalni uredaj", result.isPartialOrder ? "da" : "ne"]
            ]);
            steps("dm-relation-steps", result.steps);
            visuals?.relationGraph("dm-relation-visual", domain, pairs);
        } catch (error) { fail("dm-relation-result", error); }
    }
    document.getElementById("dm-relation-calculate").addEventListener("click", calculateRelation);

    // Teorija grafova
    function calculateGraph() {
        try {
            const adjacency = text("dm-graph-input");
            const directed = document.getElementById("dm-graph-directed").checked;
            const result = parse(module.discreteMathGraphAnalyze(adjacency, directed), "dm-graph-result");
            if (!result) return;
            render("dm-graph-result", [
                ["Stupnjevi", `[${result.degrees.join(", ")}]`, true],
                ["Povezan", result.connected ? "da" : "ne"],
                ["Eulerov ciklus", result.eulerian ? "da" : "ne"],
                ["Dvodjelan", result.bipartite ? "da" : "ne"]
            ]);
            steps("dm-graph-steps", result.steps);
            visuals?.graphDiagram("dm-graph-visual", adjacency, directed);
        } catch (error) { fail("dm-graph-result", error); }
    }
    document.getElementById("dm-graph-calculate").addEventListener("click", calculateGraph);

    function calculateShortestPath() {
        try {
            const adjacency = text("dm-path-input");
            const source = integer("dm-path-source"), targetVertex = integer("dm-path-target");
            const result = parse(module.discreteMathShortestPath(adjacency, source, targetVertex), "dm-path-result");
            if (!result) return;
            render("dm-path-result", [
                ["Dostiziv", result.reachable ? "da" : "ne", true],
                ["Udaljenost", result.reachable ? result.distance : "—"],
                ["Put", result.reachable ? `[${result.path.join(", ")}]` : "—"]
            ]);
            steps("dm-path-steps", result.steps);
            if (result.reachable) visuals?.pathDiagram("dm-path-visual", adjacency, result.path);
            else { const box = document.getElementById("dm-path-visual"); if (box) box.innerHTML = ""; }
        } catch (error) { fail("dm-path-result", error); }
    }
    document.getElementById("dm-path-calculate").addEventListener("click", calculateShortestPath);

    // Rekurzije
    function calculateRecurrence() {
        try {
            const p = Number(document.getElementById("dm-recurrence-p").value);
            const q = Number(document.getElementById("dm-recurrence-q").value);
            const a0 = Number(document.getElementById("dm-recurrence-a0").value);
            const a1 = Number(document.getElementById("dm-recurrence-a1").value);
            const result = parse(module.discreteMathSolveRecurrence(p, q, a0, a1), "dm-recurrence-result");
            if (!result) return;
            const rows = result.repeatedRoot
                ? [["Dvostruki korijen", format(result.root1), true], ["c1", format(result.coefficient1)], ["c2", format(result.coefficient2)]]
                : [["x1", format(result.root1), true], ["x2", format(result.root2)], ["c1", format(result.coefficient1)], ["c2", format(result.coefficient2)]];
            render("dm-recurrence-result", rows);
            steps("dm-recurrence-steps", result.steps);
        } catch (error) { fail("dm-recurrence-result", error); }
    }
    document.getElementById("dm-recurrence-calculate").addEventListener("click", calculateRecurrence);

    function calculateTerms() {
        try {
            const p = Number(document.getElementById("dm-recurrence-p").value);
            const q = Number(document.getElementById("dm-recurrence-q").value);
            const a0 = Number(document.getElementById("dm-recurrence-a0").value);
            const a1 = Number(document.getElementById("dm-recurrence-a1").value);
            const count = integer("dm-terms-count");
            const result = parse(module.discreteMathRecurrenceTerms(p, q, a0, a1, count), "dm-terms-result");
            if (!result) return;
            render("dm-terms-result", [["Clanovi", `[${result.terms.map(format).join(", ")}]`, true]]);
            steps("dm-terms-steps", result.steps);
            visuals?.termsChart("dm-terms-visual", result.terms);
        } catch (error) { fail("dm-terms-result", error); }
    }
    document.getElementById("dm-terms-calculate").addEventListener("click", calculateTerms);

    // Napredna kombinatorika
    function calculateInclusionExclusion() {
        try {
            const a = integer("dm-incl-a"), b = integer("dm-incl-b"), ab = integer("dm-incl-ab");
            const result = parse(module.discreteMathInclusionExclusionTwo(a, b, ab), "dm-combinatorics-result");
            if (!result) return;
            render("dm-combinatorics-result", [["|A ∪ B|", result.unionSize, true]]);
            steps("dm-combinatorics-steps", result.steps);
            visuals?.inclusionExclusion("dm-incl-visual", a, b, ab);
        } catch (error) { fail("dm-combinatorics-result", error); }
    }
    document.getElementById("dm-incl-calculate").addEventListener("click", calculateInclusionExclusion);

    function calculatePigeonhole() {
        try {
            const items = integer("dm-pigeonhole-items"), holes = integer("dm-pigeonhole-holes");
            const result = parse(module.discreteMathPigeonhole(items, holes), "dm-combinatorics-result");
            if (!result) return;
            render("dm-combinatorics-result", [["Minimum po pretincu", result.minimumPerHole, true]]);
            steps("dm-combinatorics-steps", result.steps);
        } catch (error) { fail("dm-combinatorics-result", error); }
    }
    document.getElementById("dm-pigeonhole-calculate").addEventListener("click", calculatePigeonhole);

    function calculateDerangements() {
        try {
            const n = integer("dm-derangement-n");
            const result = parse(module.discreteMathDerangements(n), "dm-combinatorics-result");
            if (!result) return;
            render("dm-combinatorics-result", [["Broj deranzmana", result.derangements, true]]);
            steps("dm-combinatorics-steps", result.steps);
        } catch (error) { fail("dm-combinatorics-result", error); }
    }
    document.getElementById("dm-derangement-calculate").addEventListener("click", calculateDerangements);

    setupDiscreteMathPractice({ root: document.getElementById("dm-practice-root"), format });
}
