"use strict";

// Pregled napretka iz localStorage (isti ključevi kao chapter-practice.js) i prijedlog sljedeceg koraka.
function setupProgressOverview() {
    const box = document.getElementById("progress-overview");
    if (!box) return;

    const BANKS = [
        { name: "Diferencijalni račun", key: "mathengine-differential-calculus-practice-v1", url: "data/differential-calculus-exercises.json", panel: "dc-practice-root" },
        { name: "Integralni račun", key: "mathengine-integral-calculus-practice-v1", url: "data/integral-calculus-exercises.json", panel: "ic-practice-root" },
        { name: "Vjerojatnost i statistika", key: "mathengine-probability-statistics-practice-v1", url: "data/probability-statistics-exercises.json", panel: "ps-practice-root" },
        { name: "Kompleksni brojevi", key: "mathengine-complex-numbers-practice-v1", url: "data/complex-numbers-exercises.json", panel: "cn-practice-root" },
    ];

    const totals = {};
    function readState(key) {
        try { return JSON.parse(localStorage.getItem(key) || "{}"); } catch (_) { return {}; }
    }

    function render() {
        const rows = BANKS.map((b) => {
            const s = readState(b.key);
            const total = totals[b.url] || 0;
            const solved = Object.keys(s.completed || {}).length;
            const attempts = s.attempts || 0;
            const accuracy = attempts ? (s.correct || 0) / attempts : null;
            return { ...b, total, solved, attempts, accuracy, points: s.points || 0 };
        });

        const table = document.createElement("div");
        table.className = "progress-rows";
        rows.forEach((r) => {
            const row = document.createElement("div");
            row.className = "progress-row";
            const label = document.createElement("span");
            label.textContent = r.name;
            const bar = document.createElement("progress");
            bar.max = r.total || 1;
            bar.value = r.solved;
            const info = document.createElement("small");
            info.textContent = `${r.solved}/${r.total || "?"} zadataka · ${r.accuracy === null ? "—" : Math.round(r.accuracy * 100) + "%"} točnost · ${r.points} bodova`;
            row.append(label, bar, info);
            table.append(row);
        });

        const started = rows.filter((r) => r.attempts > 0);
        let suggestion;
        if (!started.length) {
            suggestion = "Počni s vježbaonicom: " + rows[0].name + " (razina Temelji).";
        } else {
            const weak = started.filter((r) => r.accuracy < 0.7).sort((a, b) => a.accuracy - b.accuracy)[0];
            const open = rows.filter((r) => r.total && r.solved < r.total).sort((a, b) => a.solved / a.total - b.solved / b.total)[0];
            if (weak) suggestion = `Ponovi područje „${weak.name}” — točnost ${Math.round(weak.accuracy * 100)}% još je ispod 70%.`;
            else if (open) suggestion = `Nastavi s područjem „${open.name}” — riješeno ${open.solved} od ${open.total} zadataka.`;
            else suggestion = "Riješio si sve zadatke u svim vježbaonicama. Probaj razinu Izazov ili nove alate (Fourierovi redovi, višestruki integrali).";
        }

        const tip = document.createElement("p");
        tip.className = "progress-suggestion";
        tip.textContent = "Sljedeći korak: " + suggestion;
        box.replaceChildren(table, tip);
    }

    Promise.all(BANKS.map((b) => fetch(b.url).then((r) => r.json()).then((j) => {
        totals[b.url] = Object.values(j).reduce((n, list) => n + (Array.isArray(list) ? list.length : 0), 0);
    }).catch(() => { totals[b.url] = 0; }))).then(render);

    let timer = null;
    document.addEventListener("click", () => {
        clearTimeout(timer);
        timer = setTimeout(render, 150);
    });
}

window.setupProgressOverview = setupProgressOverview;
