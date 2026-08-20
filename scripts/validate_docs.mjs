import fs from "node:fs";
import path from "node:path";

const root = path.resolve(import.meta.dirname, "..");
const documentationFiles = [
    path.join(root, "README.md"),
    ...fs.readdirSync(path.join(root, "docs"))
        .filter((name) => name.endsWith(".md"))
        .map((name) => path.join(root, "docs", name))
];

const errors = [];
let checkedLinks = 0;
let mermaidBlocks = 0;

for (const file of documentationFiles) {
    const source = fs.readFileSync(file, "utf8");
    const relativeFile = path.relative(root, file).replaceAll("\\", "/");
    const fences = [...source.matchAll(/^```([^\r\n]*)/gm)];
    if (fences.length % 2 !== 0) errors.push(`${relativeFile}: nezatvoren code fence`);
    mermaidBlocks += fences.filter((match) => match[1].trim() === "mermaid").length;

    for (const match of source.matchAll(/\[[^\]]+\]\(([^)]+)\)/g)) {
        const rawTarget = match[1].trim();
        if (!rawTarget || rawTarget.startsWith("#") || /^[a-z]+:/i.test(rawTarget)) continue;
        const targetWithoutAnchor = rawTarget.split("#", 1)[0];
        if (!targetWithoutAnchor) continue;
        checkedLinks++;
        const resolved = path.resolve(path.dirname(file), decodeURIComponent(targetWithoutAnchor));
        if (!fs.existsSync(resolved)) errors.push(`${relativeFile}: ne postoji poveznica ${rawTarget}`);
    }
}

const requiredPaths = [
    "core/CMakeLists.txt",
    "core/src/wasm_bindings.cpp",
    "core/include/aksiomat/arithmetic/Arithmetic.hpp",
    "core/include/aksiomat/algebra/AlgebraExpression.hpp",
    "core/include/aksiomat/logic/LogicExpression.hpp",
    "core/include/aksiomat/predicate/PredicateExpression.hpp",
    "core/include/aksiomat/geometry/UnitConversion.hpp",
    "web/index.html",
    "web/app.js",
    "web/geometry.js",
    "web/geometry-visuals.js",
    "web/geometry-practice.js",
    "web/data/formalization-exercises.json",
    "web/data/geometry-exercises.json",
    "tests/CMakeLists.txt",
    ".github/workflows/ci.yml"
];

for (const relativePath of requiredPaths) {
    if (!fs.existsSync(path.join(root, relativePath))) errors.push(`Nedostaje dokumentirana putanja: ${relativePath}`);
}

if (errors.length) {
    errors.forEach((error) => console.error(error));
    process.exit(1);
}

console.log(`${documentationFiles.length} Markdown datoteka, ${checkedLinks} lokalnih poveznica, ${mermaidBlocks} Mermaid blokova i ${requiredPaths.length} ključnih putanja je valjano.`);
