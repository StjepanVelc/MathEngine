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

for (const name of ["formalization-exercises.json", "geometry-exercises.json", "trigonometry-exercises.json", "sequences-exercises.json", "analytic-geometry-exercises.json", "exponential-logarithmic-exercises.json", "combinatorics-probability-statistics-exercises.json", "calculus-basics-exercises.json"]) {
    JSON.parse(fs.readFileSync(path.join(web, "data", name), "utf8"));
}

const html = fs.readFileSync(path.join(web, "index.html"), "utf8");
const dynamicHtml = fs.readFileSync(path.join(web, "geometry-practice.js"), "utf8") + fs.readFileSync(path.join(web, "trigonometry-practice.js"), "utf8") + fs.readFileSync(path.join(web, "sequences-practice.js"), "utf8") + fs.readFileSync(path.join(web, "analytic-geometry-practice.js"), "utf8") + fs.readFileSync(path.join(web, "exponential-logarithmic-practice.js"), "utf8") + fs.readFileSync(path.join(web, "combinatorics-probability-statistics-practice.js"), "utf8") + fs.readFileSync(path.join(web, "calculus-basics-practice.js"), "utf8");
const allMarkup = `${html}\n${dynamicHtml}`;
const staticIds = [...html.matchAll(/\bid="([^"]+)"/g)].map((match) => match[1]);
const duplicates = staticIds.filter((id, index) => staticIds.indexOf(id) !== index);
if (duplicates.length) {
    throw new Error(`Duplicate static HTML IDs: ${[...new Set(duplicates)].join(", ")}`);
}

const referencedIds = new Set();
for (const name of ["geometry.js", "geometry-practice.js", "geometry-visuals.js", "trigonometry.js", "trigonometry-practice.js", "trigonometry-visuals.js", "sequences.js", "sequences-practice.js", "sequences-visuals.js", "analytic-geometry.js", "analytic-geometry-practice.js", "analytic-geometry-visuals.js", "exponential-logarithmic.js", "exponential-logarithmic-practice.js", "exponential-logarithmic-visuals.js", "combinatorics-probability-statistics.js", "combinatorics-probability-statistics-practice.js", "combinatorics-probability-statistics-visuals.js", "calculus-basics.js", "calculus-basics-practice.js", "calculus-basics-visuals.js"]) {
    const source = fs.readFileSync(path.join(web, name), "utf8");
    for (const match of source.matchAll(/getElementById\("([^"]+)"\)/g)) referencedIds.add(match[1]);
}
const dynamicFieldIds = new Set([
    "trigonometry-right-first",
    "trigonometry-right-second",
    "trigonometry-right-side",
    "trigonometry-general-first",
    "trigonometry-general-second",
    "trigonometry-general-third",
    "analytic-circle-first", "analytic-circle-second", "analytic-circle-third",
    "analytic-circle-fourth", "analytic-circle-fifth", "analytic-circle-sixth",
    "explog-powers-exponent", "explog-powers-degree",
    "explog-application-first", "explog-application-second", "explog-application-third",
    "cps-counting-n", "cps-counting-k",
    "cps-probability-first", "cps-probability-second", "cps-probability-third"
]);
const cbDynamicIds = ["cb-practice-points", "cb-practice-correct", "cb-practice-accuracy", "cb-practice-best", "cb-practice-topic", "cb-practice-title", "cb-practice-question", "cb-practice-unit", "cb-practice-streak", "cb-practice-answer", "cb-practice-feedback", "cb-practice-hint-box", "cb-practice-solution", "cb-practice-check", "cb-practice-hint", "cb-practice-next", "cb-practice-reset"];
for (const id of cbDynamicIds) dynamicFieldIds.add(id);
const missing = [...referencedIds].filter((id) => !allMarkup.includes(`id="${id}"`) && !dynamicFieldIds.has(id));
if (missing.length) throw new Error(`Missing domain DOM IDs: ${missing.join(", ")}`);

const exercises = JSON.parse(fs.readFileSync(path.join(web, "data", "geometry-exercises.json"), "utf8"));
const allExercises = ["lagano", "srednje", "izazov"].flatMap((level) => exercises[level] || []);
if (allExercises.length !== 22) throw new Error(`Expected 22 Geometry exercises, found ${allExercises.length}`);
if (new Set(allExercises.map((exercise) => exercise.id)).size !== allExercises.length) {
    throw new Error("Geometry exercise IDs must be unique");
}

const trigonometryExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "trigonometry-exercises.json"), "utf8"));
const allTrigonometryExercises = ["temelji", "primjena", "izazov"].flatMap((level) => trigonometryExercises[level] || []);
if (allTrigonometryExercises.length !== 24) throw new Error(`Expected 24 Trigonometry exercises, found ${allTrigonometryExercises.length}`);
if (new Set(allTrigonometryExercises.map((exercise) => exercise.id)).size !== allTrigonometryExercises.length) throw new Error("Trigonometry exercise IDs must be unique");

const sequencesExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "sequences-exercises.json"), "utf8"));
const allSequencesExercises = ["temelji", "primjena", "izazov"].flatMap((level) => sequencesExercises[level] || []);
if (allSequencesExercises.length !== 24) throw new Error(`Expected 24 Sequences exercises, found ${allSequencesExercises.length}`);
if (new Set(allSequencesExercises.map((exercise) => exercise.id)).size !== allSequencesExercises.length) throw new Error("Sequences exercise IDs must be unique");
const requiredExerciseFields = ["id", "topic", "title", "question", "answer", "unit", "tolerance", "hint", "explanation"];
for (const exercise of allSequencesExercises) {
    for (const field of requiredExerciseFields) if (!(field in exercise)) throw new Error(`Sequences exercise ${exercise.id || "<unknown>"} is missing ${field}`);
    if (!Number.isFinite(exercise.answer) || !Number.isFinite(exercise.tolerance) || exercise.tolerance < 0) throw new Error(`Sequences exercise ${exercise.id} has invalid numeric validation`);
}

const analyticExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "analytic-geometry-exercises.json"), "utf8"));
const allAnalyticExercises = ["temelji", "primjena", "izazov"].flatMap((level) => analyticExercises[level] || []);
if (allAnalyticExercises.length !== 24) throw new Error(`Expected 24 Analytic Geometry exercises, found ${allAnalyticExercises.length}`);
if (new Set(allAnalyticExercises.map((exercise) => exercise.id)).size !== allAnalyticExercises.length) throw new Error("Analytic Geometry exercise IDs must be unique");
for (const exercise of allAnalyticExercises) {
    for (const field of requiredExerciseFields) if (!(field in exercise)) throw new Error(`Analytic Geometry exercise ${exercise.id || "<unknown>"} is missing ${field}`);
    if (!Number.isFinite(exercise.answer) || !Number.isFinite(exercise.tolerance) || exercise.tolerance < 0) throw new Error(`Analytic Geometry exercise ${exercise.id} has invalid numeric validation`);
}

const explogExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "exponential-logarithmic-exercises.json"), "utf8"));
const allExplogExercises = ["temelji", "primjena", "izazov"].flatMap((level) => explogExercises[level] || []);
if (allExplogExercises.length !== 24) throw new Error(`Expected 24 Exponential/Logarithmic exercises, found ${allExplogExercises.length}`);
if (new Set(allExplogExercises.map((exercise) => exercise.id)).size !== allExplogExercises.length) throw new Error("Exponential/Logarithmic exercise IDs must be unique");
for (const exercise of allExplogExercises) {
    for (const field of requiredExerciseFields) if (!(field in exercise)) throw new Error(`Exponential/Logarithmic exercise ${exercise.id || "<unknown>"} is missing ${field}`);
    if (!Number.isFinite(exercise.answer) || !Number.isFinite(exercise.tolerance) || exercise.tolerance < 0) throw new Error(`Exponential/Logarithmic exercise ${exercise.id} has invalid numeric validation`);
}

const cpsExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "combinatorics-probability-statistics-exercises.json"), "utf8"));
const allCpsExercises = ["temelji", "primjena", "izazov"].flatMap((level) => cpsExercises[level] || []);
if (allCpsExercises.length !== 24) throw new Error(`Expected 24 Combinatorics/Probability/Statistics exercises, found ${allCpsExercises.length}`);
if (new Set(allCpsExercises.map((exercise) => exercise.id)).size !== allCpsExercises.length) throw new Error("Combinatorics/Probability/Statistics exercise IDs must be unique");
for (const exercise of allCpsExercises) {
    for (const field of requiredExerciseFields) if (!(field in exercise)) throw new Error(`Combinatorics/Probability/Statistics exercise ${exercise.id || "<unknown>"} is missing ${field}`);
    if (!Number.isFinite(exercise.answer) || !Number.isFinite(exercise.tolerance) || exercise.tolerance < 0) throw new Error(`Combinatorics/Probability/Statistics exercise ${exercise.id} has invalid numeric validation`);
}

const calculusExercises = JSON.parse(fs.readFileSync(path.join(web, "data", "calculus-basics-exercises.json"), "utf8"));
const allCalculusExercises = ["temelji", "primjena", "izazov"].flatMap((level) => calculusExercises[level] || []);
if (allCalculusExercises.length !== 24) throw new Error(`Expected 24 Calculus Basics exercises, found ${allCalculusExercises.length}`);
if (new Set(allCalculusExercises.map((exercise) => exercise.id)).size !== allCalculusExercises.length) throw new Error("Calculus Basics exercise IDs must be unique");
for (const exercise of allCalculusExercises) {
    for (const field of requiredExerciseFields) if (!(field in exercise)) throw new Error(`Calculus Basics exercise ${exercise.id || "<unknown>"} is missing ${field}`);
    if (!Number.isFinite(exercise.answer) || !Number.isFinite(exercise.tolerance) || exercise.tolerance < 0) throw new Error(`Calculus Basics exercise ${exercise.id} has invalid numeric validation`);
}

console.log(`${scriptFiles.length} JavaScript files, 8 JSON banks, ${staticIds.length} static IDs, ${referencedIds.size} domain references, 22 Geometry and 24 exercises in each other secondary-school practice bank valid.`);
