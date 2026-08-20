import fs from "node:fs";
import path from "node:path";
import vm from "node:vm";

const root = path.resolve(import.meta.dirname, "..");
const web = path.join(root, "web");
const scriptFiles = fs.readdirSync(web).filter((name) => name.endsWith(".js"));

for (const name of scriptFiles) {
    const source = fs.readFileSync(path.join(web, name), "utf8");
    new vm.Script(source, { filename: name });
}

for (const name of ["formalization-exercises.json", "geometry-exercises.json"]) {
    JSON.parse(fs.readFileSync(path.join(web, "data", name), "utf8"));
}

const html = fs.readFileSync(path.join(web, "index.html"), "utf8");
const dynamicHtml = fs.readFileSync(path.join(web, "geometry-practice.js"), "utf8");
const allMarkup = `${html}\n${dynamicHtml}`;
const staticIds = [...html.matchAll(/\bid="([^"]+)"/g)].map((match) => match[1]);
const duplicates = staticIds.filter((id, index) => staticIds.indexOf(id) !== index);
if (duplicates.length) {
    throw new Error(`Duplicate static HTML IDs: ${[...new Set(duplicates)].join(", ")}`);
}

const referencedIds = new Set();
for (const name of ["geometry.js", "geometry-practice.js", "geometry-visuals.js"]) {
    const source = fs.readFileSync(path.join(web, name), "utf8");
    for (const match of source.matchAll(/getElementById\("([^"]+)"\)/g)) referencedIds.add(match[1]);
}
const missing = [...referencedIds].filter((id) => !allMarkup.includes(`id="${id}"`));
if (missing.length) throw new Error(`Missing Geometry DOM IDs: ${missing.join(", ")}`);

const exercises = JSON.parse(fs.readFileSync(path.join(web, "data", "geometry-exercises.json"), "utf8"));
const allExercises = ["lagano", "srednje", "izazov"].flatMap((level) => exercises[level] || []);
if (allExercises.length !== 22) throw new Error(`Expected 22 Geometry exercises, found ${allExercises.length}`);
if (new Set(allExercises.map((exercise) => exercise.id)).size !== allExercises.length) {
    throw new Error("Geometry exercise IDs must be unique");
}

console.log(`${scriptFiles.length} JavaScript files, 2 JSON banks, ${staticIds.length} static IDs, ${referencedIds.size} Geometry references and 22 exercises valid.`);
