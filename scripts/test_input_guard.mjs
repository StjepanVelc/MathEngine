// Testovi za web/input-guard.js: globalnu zastitu unosa (blokiranje '<'/'>' i
// ogranicenje duljine) te formatNumberForDisplay (znanstveni zapis ekstremnih brojeva).
//
// Ucitava izvorni kod u vm kontekst s minimalnim "document"/"window" stubovima
// kako bi se IIFE iz input-guard.js mogao izvrsiti izvan preglednika, a zatim
// testira izlozene funkcije (window.guardValue i globalni formatNumberForDisplay)
// nad rucno napravljenim "input" objektima.

import fs from "node:fs";
import path from "node:path";
import vm from "node:vm";
import assert from "node:assert/strict";

const root = path.resolve(import.meta.dirname, "..");
const source = fs.readFileSync(path.join(root, "web", "input-guard.js"), "utf8");

function makeInput(type, value) {
    return {
        tagName: "INPUT",
        type,
        value,
        getAttribute(name) {
            return name === "type" ? type : null;
        },
    };
}

function loadInputGuard() {
    const listeners = {};
    const context = {
        document: {
            addEventListener(event, handler) {
                listeners[event] = handler;
            },
            activeElement: null,
        },
        window: {},
        setTimeout,
        console,
    };
    vm.createContext(context);
    new vm.Script(source, { filename: "input-guard.js" }).runInContext(context);
    return { context, listeners };
}

let passed = 0;
function check(name, fn) {
    fn();
    passed += 1;
    console.log(`ok - ${name}`);
}

const { context } = loadInputGuard();
const { guardValue } = context.window;
const { formatNumberForDisplay } = context;

assert.equal(typeof guardValue, "function", "window.guardValue must be exposed");
assert.equal(typeof formatNumberForDisplay, "function", "formatNumberForDisplay must be exposed globally");

check("strips angle brackets from text input", () => {
    const el = makeInput("text", "x^2 <script>alert(1)</script>");
    guardValue(el);
    assert.ok(!el.value.includes("<"));
    assert.ok(!el.value.includes(">"));
    assert.equal(el.value, "x^2 scriptalert(1)/script");
});

check("truncates text input longer than 200 characters", () => {
    const long = "a".repeat(250);
    const el = makeInput("text", long);
    guardValue(el);
    assert.equal(el.value.length, 200);
});

check("truncates number input longer than 32 characters", () => {
    const long = "9".repeat(50);
    const el = makeInput("number", long);
    guardValue(el);
    assert.equal(el.value.length, 32);
});

check("leaves legitimate unicode math syntax untouched", () => {
    const original = "(p \u2227 \u00acq) \u2192 r, \u2200x \u2203y Manji(x, y)";
    const el = makeInput("text", original);
    guardValue(el);
    assert.equal(el.value, original);
});

check("ignores non-guarded input types", () => {
    const el = makeInput("range", "<>".repeat(50));
    const before = el.value;
    guardValue(el);
    assert.equal(el.value, before);
});

check("ignores non-input elements", () => {
    const el = { tagName: "DIV", value: "<x>" };
    assert.doesNotThrow(() => guardValue(el));
    assert.equal(el.value, "<x>");
});

check("formatNumberForDisplay keeps small/medium numbers as plain decimals", () => {
    assert.equal(formatNumberForDisplay(0), "0");
    assert.equal(formatNumberForDisplay(3.14159, 2), "3.14");
    assert.equal(formatNumberForDisplay(-42), "-42");
});

check("formatNumberForDisplay switches to scientific notation for very large numbers", () => {
    const result = formatNumberForDisplay(123456789, 3);
    assert.match(result, /^1\.235e\+8$/);
});

check("formatNumberForDisplay switches to scientific notation for very small numbers", () => {
    const result = formatNumberForDisplay(0.0000001234, 3);
    assert.match(result, /^1\.234e-7$/);
});

check("formatNumberForDisplay handles non-finite input gracefully", () => {
    assert.equal(formatNumberForDisplay(Infinity), "Infinity");
    assert.equal(formatNumberForDisplay(NaN), "NaN");
    assert.equal(formatNumberForDisplay("not-a-number"), "not-a-number");
});

console.log(`${passed} input-guard tests passed.`);
