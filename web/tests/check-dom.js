// Statička provjera web sučelja: duplicirani ID-ovi, data-panel bez panela i ID-ovi iz JS-a koji ne postoje.
// ID-ovi koje JS gradi u predlošcima (npr. id="ma-practice-points" ili id="${prefix}-practice-x") računaju se kao definirani.
// Pokretanje: node web/tests/check-dom.js
const fs = require("fs");
const path = require("path");

const webDir = path.resolve(__dirname, "..");
const pages = process.argv.length > 2 ? process.argv.slice(2) : ["fakultet.html"];
const lookup = /(?:getElementById|\$|number|text|select|num|byId)\(\s*["']([A-Za-z][A-Za-z0-9_]*-[A-Za-z0-9_-]+)["']/g;
const prefixed = /getElementById\(\s*prefix\s*\+\s*["']([A-Za-z0-9_-]+)["']/g;
let errors = 0;

for (const page of pages) {
    const html = fs.readFileSync(path.join(webDir, page), "utf8");
    const ids = [...html.matchAll(/\sid\s*=\s*"([^"]+)"/g)].map((m) => m[1]);
    const idSet = new Set(ids);

    const seen = new Set();
    for (const id of ids) {
        if (seen.has(id)) { console.error(`[${page}] duplicirani id: ${id}`); errors++; }
        seen.add(id);
    }

    const scripts = [...html.matchAll(/<script[^>]+src="([^"]+\.js)"/g)].map((m) => m[1]).filter((s) => !/^https?:/.test(s));
    const sources = [];
    for (const script of scripts) {
        const file = path.join(webDir, script);
        if (!fs.existsSync(file)) { console.error(`[${page}] nedostaje skripta: ${script}`); errors++; continue; }
        const js = fs.readFileSync(file, "utf8");
        sources.push([script, js]);
        for (const m of js.matchAll(/id=\\?"([A-Za-z][^"\\<>$]*)\\?"/g)) idSet.add(m[1]);
        for (const m of js.matchAll(/\.id\s*=\s*["']([^"']+)["']/g)) idSet.add(m[1]);
    }
    const dynamicSuffixes = new Set();
    for (const [, js] of sources) for (const m of js.matchAll(/id=\\?"\$\{[^}]+\}(-[A-Za-z0-9-]+)\\?"/g)) dynamicSuffixes.add(m[1]);

    for (const [script, js] of sources) {
        for (const m of js.matchAll(lookup)) {
            const id = m[1];
            if (idSet.has(id)) continue;
            if ([...dynamicSuffixes].some((s) => id.endsWith(s))) continue;
            if ([...idSet].some((x) => x.split("-")[0] === id.split("-")[0])) {
                console.error(`[${page}] ${script}: id "${id}" ne postoji u HTML-u`);
                errors++;
            }
        }
        for (const m of js.matchAll(prefixed)) {
            if (!dynamicSuffixes.has(m[1])) { console.error(`[${page}] ${script}: prefiksirani id "${m[1]}" nije definiran u predlošku`); errors++; }
        }
    }

    for (const m of html.matchAll(/data-panel="([^"]+)"/g)) {
        if (!idSet.has(m[1])) { console.error(`[${page}] data-panel="${m[1]}" nema odgovarajući panel`); errors++; }
    }
}

for (const file of fs.readdirSync(path.join(webDir, "data")).filter((f) => f.endsWith(".json"))) {
    try { JSON.parse(fs.readFileSync(path.join(webDir, "data", file), "utf8")); }
    catch (e) { console.error(`data/${file}: neispravan JSON (${e.message})`); errors++; }
}

if (errors) { console.error(`\nDOM provjera: ${errors} problem(a).`); process.exit(1); }
console.log(`DOM provjera: OK (${pages.length} stranica).`);
