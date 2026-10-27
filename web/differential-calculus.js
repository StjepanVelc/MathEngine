"use strict";

// Diferencijalni racun i limesi: UI sloj. Sva matematika je u C++ jezgri (diffCalc* funkcije).
function setupDifferentialCalculus(module) {
    const root = document.querySelector(".differential-calculus-chapter");
    if (!root || typeof module.diffCalcLimit !== "function") return;

    const $ = (id) => document.getElementById(id);
    const COLORS = { f: "#2563eb", df: "#ea580c", ddf: "#16a34a", axis: "#94a3b8", grid: "#e2e8f0", mark: "#dc2626", text: "#334155" };

    const fmt = (v) => (v === null || v === undefined || !Number.isFinite(v)) ? "—" : new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(v);

    function call(fn, resultEl, stepsEl) {
        resultEl.replaceChildren();
        if (stepsEl) stepsEl.replaceChildren();
        let text;
        try { text = fn(); } catch (e) { fail(resultEl, String(e)); return null; }
        if (typeof text === "string" && text.startsWith("GRESKA:")) { fail(resultEl, text.slice(7).trim()); return null; }
        try { return JSON.parse(text); } catch (e) { fail(resultEl, "Neispravan odgovor jezgre."); return null; }
    }

    function fail(el, message) {
        const p = document.createElement("p");
        p.className = "error";
        p.textContent = message;
        el.replaceChildren(p);
    }

    function card(label, value) {
        const div = document.createElement("div");
        div.className = "result-card";
        const l = document.createElement("span");
        l.textContent = label;
        const v = document.createElement("strong");
        v.textContent = value;
        div.append(l, v);
        return div;
    }

    function showCards(el, pairs) {
        el.replaceChildren(...pairs.map(([l, v]) => card(l, v)));
    }

    function showSteps(el, steps) {
        el.replaceChildren(...steps.map((s, i) => {
            const row = document.createElement("div");
            row.className = "mathematical-analysis-step";
            const n = document.createElement("span");
            n.textContent = String(i + 1);
            const p = document.createElement("p");
            p.textContent = s;
            row.append(n, p);
            return row;
        }));
    }

    function limitText(l) {
        if (l.kind === "finite") return fmt(l.value);
        return l.text;
    }

    // ---- Canvas pomocnici ----
    function makeView(canvas, xMin, xMax, yMin, yMax) {
        const w = canvas.width, h = canvas.height, pad = 32;
        if (!(yMax > yMin)) { yMax = yMin + 1; }
        return {
            w, h, pad, xMin, xMax, yMin, yMax,
            X: (x) => pad + (x - xMin) / (xMax - xMin) * (w - 2 * pad),
            Y: (y) => h - pad - (y - yMin) / (yMax - yMin) * (h - 2 * pad),
        };
    }

    function niceStep(range) {
        const raw = range / 8;
        const p = Math.pow(10, Math.floor(Math.log10(raw)));
        const n = raw / p;
        return (n < 1.5 ? 1 : n < 3.5 ? 2 : n < 7.5 ? 5 : 10) * p;
    }

    function drawAxes(ctx, v) {
        ctx.clearRect(0, 0, v.w, v.h);
        ctx.fillStyle = "#fff";
        ctx.fillRect(0, 0, v.w, v.h);
        ctx.font = "11px sans-serif";
        ctx.fillStyle = COLORS.text;
        ctx.strokeStyle = COLORS.grid;
        ctx.lineWidth = 1;
        const sx = niceStep(v.xMax - v.xMin), sy = niceStep(v.yMax - v.yMin);
        for (let x = Math.ceil(v.xMin / sx) * sx; x <= v.xMax; x += sx) {
            ctx.beginPath(); ctx.moveTo(v.X(x), v.pad); ctx.lineTo(v.X(x), v.h - v.pad); ctx.stroke();
            ctx.fillText(fmt(+x.toFixed(6)), v.X(x) - 8, v.h - v.pad + 14);
        }
        for (let y = Math.ceil(v.yMin / sy) * sy; y <= v.yMax; y += sy) {
            ctx.beginPath(); ctx.moveTo(v.pad, v.Y(y)); ctx.lineTo(v.w - v.pad, v.Y(y)); ctx.stroke();
            ctx.fillText(fmt(+y.toFixed(6)), 2, v.Y(y) + 4);
        }
        ctx.strokeStyle = COLORS.axis;
        ctx.lineWidth = 1.5;
        if (v.xMin <= 0 && v.xMax >= 0) { ctx.beginPath(); ctx.moveTo(v.X(0), v.pad); ctx.lineTo(v.X(0), v.h - v.pad); ctx.stroke(); }
        if (v.yMin <= 0 && v.yMax >= 0) { ctx.beginPath(); ctx.moveTo(v.pad, v.Y(0)); ctx.lineTo(v.w - v.pad, v.Y(0)); ctx.stroke(); }
    }

    function drawCurve(ctx, v, pts, color, width) {
        ctx.strokeStyle = color;
        ctx.lineWidth = width || 2;
        ctx.beginPath();
        let pen = false, prevY = null;
        for (const [x, y] of pts) {
            if (y === null || !Number.isFinite(y)) { pen = false; prevY = null; continue; }
            const py = v.Y(y);
            const jump = prevY !== null && Math.abs(py - prevY) > v.h * 0.8;
            if (!pen || jump) ctx.moveTo(v.X(x), py); else ctx.lineTo(v.X(x), py);
            pen = true; prevY = py;
        }
        ctx.stroke();
    }

    function drawPoint(ctx, v, x, y, color, label, hollow) {
        if (!Number.isFinite(y)) return;
        ctx.beginPath();
        ctx.arc(v.X(x), v.Y(y), 5, 0, 2 * Math.PI);
        if (hollow) { ctx.fillStyle = "#fff"; ctx.fill(); ctx.strokeStyle = color; ctx.lineWidth = 2; ctx.stroke(); }
        else { ctx.fillStyle = color; ctx.fill(); }
        if (label) { ctx.fillStyle = COLORS.text; ctx.font = "11px sans-serif"; ctx.fillText(label, v.X(x) + 7, v.Y(y) - 7); }
    }

    function drawLine(ctx, v, slope, x0, y0, color, dash) {
        ctx.strokeStyle = color;
        ctx.lineWidth = 1.8;
        ctx.setLineDash(dash || []);
        ctx.beginPath();
        ctx.moveTo(v.X(v.xMin), v.Y(y0 + slope * (v.xMin - x0)));
        ctx.lineTo(v.X(v.xMax), v.Y(y0 + slope * (v.xMax - x0)));
        ctx.stroke();
        ctx.setLineDash([]);
    }

    function rangeY(pts, cap) {
        let lo = Infinity, hi = -Infinity;
        for (const [, y] of pts) if (y !== null && Number.isFinite(y) && Math.abs(y) < cap) { lo = Math.min(lo, y); hi = Math.max(hi, y); }
        if (!Number.isFinite(lo)) { lo = -1; hi = 1; }
        const m = (hi - lo) * 0.12 || 1;
        return [lo - m, hi + m];
    }

    function sampleCurve(expr, a, b, n) {
        const text = module.diffCalcSample(expr, a, b, n);
        if (text.startsWith("GRESKA:")) return [];
        return JSON.parse(text);
    }

    function toggles() {
        const buttons = root.querySelectorAll(".dc-toggle");
        const panels = root.querySelectorAll(".dc-panel");
        function show(id) {
            panels.forEach((p) => { p.hidden = p.id !== id; });
            buttons.forEach((b) => { b.classList.toggle("active", b.dataset.panel === id); b.setAttribute("aria-pressed", String(b.dataset.panel === id)); });
        }
        buttons.forEach((b) => b.addEventListener("click", () => show(b.dataset.panel)));
        show("dc-limit");
    }

    // ---- Limes ----
    function runLimit() {
        const expr = $("dc-limit-expression").value, point = $("dc-limit-point").value;
        const r = call(() => module.diffCalcLimit(expr, point), $("dc-limit-result"), $("dc-limit-steps"));
        $("dc-limit-table").replaceChildren();
        if (!r) return;
        showCards($("dc-limit-result"), [
            ["f(x)", r.expression],
            ["Lijevi limes", limitText(r.left)],
            ["Desni limes", limitText(r.right)],
            ["Limes", limitText(r.combined)],
            ["Neodređeni oblik", r.indeterminateForm || "nema"],
        ]);
        showSteps($("dc-limit-steps"), r.steps);

        const table = document.createElement("table");
        const head = table.createTHead().insertRow();
        ["x (slijeva)", "f(x)", "x (zdesna)", "f(x)"].forEach((t) => { const th = document.createElement("th"); th.textContent = t; head.appendChild(th); });
        const body = table.createTBody();
        for (let i = 0; i < r.leftSamples.length; i++) {
            const row = body.insertRow();
            const l = r.leftSamples[i], rr = r.rightSamples[i];
            [fmt(l.x), fmt(l.y), fmt(rr.x), fmt(rr.y)].forEach((t) => { row.insertCell().textContent = t; });
        }
        $("dc-limit-table").appendChild(table);

        const canvas = $("dc-limit-canvas"), ctx = canvas.getContext("2d");
        let a = r.point, span = 3;
        if (r.atInfinity) { span = 30; a = r.pointSign > 0 ? 25 : -25; }
        const x0 = a - span, x1 = a + span;
        const pts = sampleCurve(expr, x0, x1, 600);
        const [lo, hi] = rangeY(pts, 1e3);
        const v = makeView(canvas, x0, x1, lo, hi);
        drawAxes(ctx, v);
        drawCurve(ctx, v, pts, COLORS.f, 2.5);
        if (!r.atInfinity) {
            ctx.strokeStyle = COLORS.mark; ctx.setLineDash([5, 4]); ctx.lineWidth = 1;
            ctx.beginPath(); ctx.moveTo(v.X(a), v.pad); ctx.lineTo(v.X(a), v.h - v.pad); ctx.stroke(); ctx.setLineDash([]);
            if (r.combined.kind === "finite") {
                ctx.beginPath(); ctx.moveTo(v.pad, v.Y(r.combined.value)); ctx.lineTo(v.w - v.pad, v.Y(r.combined.value)); ctx.stroke();
                drawPoint(ctx, v, a, r.combined.value, COLORS.mark, "limes = " + fmt(r.combined.value), !r.valueDefined || Math.abs(r.valueAtPoint - r.combined.value) > 1e-6);
            }
            if (r.valueDefined) drawPoint(ctx, v, a, r.valueAtPoint, COLORS.f, "f(a)", false);
        } else if (r.combined.kind === "finite") {
            ctx.strokeStyle = COLORS.mark; ctx.setLineDash([5, 4]);
            ctx.beginPath(); ctx.moveTo(v.pad, v.Y(r.combined.value)); ctx.lineTo(v.w - v.pad, v.Y(r.combined.value)); ctx.stroke(); ctx.setLineDash([]);
        }
    }

    // ---- Neprekidnost ----
    const CONT_NAMES = {
        continuous: "neprekidna", removable: "uklonjiv prekid", jump: "skok", infinite: "beskonačni prekid",
        oscillating: "oscilirajući prekid", outside_domain: "točka izvan domene",
    };
    function runContinuity() {
        const expr = $("dc-cont-expression").value, a = Number($("dc-cont-point").value);
        if ($("dc-cont-point").value === "" || !Number.isFinite(a)) { fail($("dc-cont-result"), "Unesi broj za točku a."); return; }
        const r = call(() => module.diffCalcContinuity(expr, a), $("dc-cont-result"), $("dc-cont-steps"));
        if (!r) return;
        showCards($("dc-cont-result"), [
            ["f(a)", r.valueDefined ? fmt(r.value) : "nije definiran"],
            ["Lijevi limes", limitText(r.left)],
            ["Desni limes", limitText(r.right)],
            ["Zaključak", CONT_NAMES[r.kind] || r.kind],
        ]);
        showSteps($("dc-cont-steps"), r.steps);
        const canvas = $("dc-cont-canvas"), ctx = canvas.getContext("2d");
        const pts = sampleCurve(expr, a - 3, a + 3, 600);
        const [lo, hi] = rangeY(pts, 1e3);
        const v = makeView(canvas, a - 3, a + 3, lo, hi);
        drawAxes(ctx, v);
        drawCurve(ctx, v, pts, COLORS.f, 2.5);
        ctx.strokeStyle = COLORS.mark; ctx.setLineDash([5, 4]); ctx.lineWidth = 1;
        ctx.beginPath(); ctx.moveTo(v.X(a), v.pad); ctx.lineTo(v.X(a), v.h - v.pad); ctx.stroke(); ctx.setLineDash([]);
        if (r.left.kind === "finite") drawPoint(ctx, v, a, r.left.value, COLORS.df, "slijeva", true);
        if (r.right.kind === "finite") drawPoint(ctx, v, a, r.right.value, COLORS.ddf, "zdesna", true);
        if (r.valueDefined) drawPoint(ctx, v, a, r.value, COLORS.f, "f(a)", false);
    }

    // ---- Tangenta ----
    let tanData = null, tanExpr = "";
    function drawTangent() {
        if (!tanData) return;
        const r = tanData, a = r.point;
        const canvas = $("dc-tan-canvas"), ctx = canvas.getContext("2d");
        const h = Math.pow(10, Number($("dc-tan-h").value)) * (Number($("dc-tan-h").dataset.sign) || 1);
        $("dc-tan-h-label").textContent = fmt(h);
        const pts = sampleCurve(tanExpr, a - 3, a + 3, 600);
        const [lo, hi] = rangeY(pts, 1e3);
        const v = makeView(canvas, a - 3, a + 3, lo, hi);
        drawAxes(ctx, v);
        drawCurve(ctx, v, pts, COLORS.f, 2.5);
        if (!r.valueDefined) return;
        if (r.differentiable) drawLine(ctx, v, r.slope, a, r.value, COLORS.mark, [6, 4]);
        const near = sampleCurve(tanExpr, a + h, a + h + 1e-9 + Math.abs(h) * 1e-9, 2);
        let y1 = near.length ? near[0][1] : null;
        if (y1 !== null && Number.isFinite(y1) && h !== 0) {
            const slope = (y1 - r.value) / h;
            drawLine(ctx, v, slope, a, r.value, COLORS.df, []);
            drawPoint(ctx, v, a + h, y1, COLORS.df, "sekanta, nagib " + fmt(slope), false);
        }
        drawPoint(ctx, v, a, r.value, COLORS.f, "(a, f(a))", false);
    }

    function runTangent() {
        const expr = $("dc-tan-expression").value, a = Number($("dc-tan-point").value);
        if ($("dc-tan-point").value === "" || !Number.isFinite(a)) { fail($("dc-tan-result"), "Unesi broj za točku a."); return; }
        const r = call(() => module.diffCalcTangent(expr, a), $("dc-tan-result"), $("dc-tan-steps"));
        $("dc-tan-table").replaceChildren();
        if (!r) return;
        tanData = r; tanExpr = expr;
        const problems = { kink: "šiljak: derivacija ne postoji", vertical: "okomita tangenta", one_sided: "samo jednostrana derivacija", undefined: "f(a) nije definiran" };
        showCards($("dc-tan-result"), [
            ["f′(x)", r.derivative],
            ["f′(a)", r.differentiable ? fmt(r.slope) : (problems[r.problem] || "ne postoji")],
            ["Tangenta", r.tangent || "—"],
            ["Normala", r.normal || "—"],
        ]);
        showSteps($("dc-tan-steps"), r.steps);
        const table = document.createElement("table");
        const head = table.createTHead().insertRow();
        ["h", "x = a + h", "nagib sekante"].forEach((t) => { const th = document.createElement("th"); th.textContent = t; head.appendChild(th); });
        const body = table.createTBody();
        for (const row of r.leftRows.concat(r.rightRows)) {
            const tr = body.insertRow();
            [fmt(row.h), fmt(row.x), fmt(row.slope)].forEach((t) => { tr.insertCell().textContent = t; });
        }
        $("dc-tan-table").appendChild(table);
        drawTangent();
    }

    // ---- Pravila ----
    function runRules() {
        const expr = $("dc-rules-expression").value, order = Number($("dc-rules-order").value);
        const r = call(() => module.diffCalcDerivative(expr, Math.trunc(order)), $("dc-rules-result"), $("dc-rules-steps"));
        if (!r) return;
        showCards($("dc-rules-result"), [["f(x)", r.expression], ["Red", String(r.order)], ["Prije pojednostavljenja", r.raw], ["Derivacija", r.derivative]]);
        showSteps($("dc-rules-steps"), r.steps);
    }

    // ---- Rolle / Lagrange ----
    function runMean() {
        const expr = $("dc-mean-expression").value, a = Number($("dc-mean-a").value), b = Number($("dc-mean-b").value);
        if ($("dc-mean-a").value === "" || $("dc-mean-b").value === "") { fail($("dc-mean-result"), "Unesi a i b."); return; }
        const r = call(() => module.diffCalcMeanValue(expr, a, b), $("dc-mean-result"), $("dc-mean-steps"));
        if (!r) return;
        showCards($("dc-mean-result"), [
            ["Neprekidna na [a, b]", r.continuous ? "da" : "ne"],
            ["Derivabilna na (a, b)", r.differentiable ? "da" : "ne"],
            ["Teorem", r.rolle ? "Rolle (f(a) = f(b))" : (r.lagrange ? "Lagrange" : "uvjeti nisu ispunjeni")],
            ["Nagib sekante", fmt(r.meanSlope)],
            ["Točke c", r.points.length ? r.points.map(fmt).join("; ") : "—"],
        ]);
        showSteps($("dc-mean-steps"), r.steps);
        const canvas = $("dc-mean-canvas"), ctx = canvas.getContext("2d");
        const pad = (b - a) * 0.25 || 1;
        const pts = sampleCurve(expr, a - pad, b + pad, 600);
        const [lo, hi] = rangeY(pts, 1e3);
        const v = makeView(canvas, a - pad, b + pad, lo, hi);
        drawAxes(ctx, v);
        drawCurve(ctx, v, pts, COLORS.f, 2.5);
        drawLine(ctx, v, r.meanSlope, a, r.fa, COLORS.df, []);
        r.points.forEach((c, i) => {
            drawLine(ctx, v, r.meanSlope, c, r.pointValues[i], COLORS.mark, [6, 4]);
            drawPoint(ctx, v, c, r.pointValues[i], COLORS.mark, "c = " + fmt(c), false);
        });
        drawPoint(ctx, v, a, r.fa, COLORS.f, "a", false);
        drawPoint(ctx, v, b, r.fb, COLORS.f, "b", false);
    }

    // ---- Tok funkcije ----
    const BEHAVIOR = { increasing: "raste", decreasing: "pada", constant: "konstantna", convex: "konveksna", concave: "konkavna", linear: "pravac" };
    function runStudy() {
        const expr = $("dc-study-expression").value, a = Number($("dc-study-min").value), b = Number($("dc-study-max").value);
        if ($("dc-study-min").value === "" || $("dc-study-max").value === "") { fail($("dc-study-result"), "Unesi interval."); return; }
        const r = call(() => module.diffCalcStudy(expr, a, b), $("dc-study-result"), $("dc-study-steps"));
        if (!r) return;
        const ints = (list) => list.length ? list.map((i) => "(" + fmt(i.from) + ", " + fmt(i.to) + ") " + (BEHAVIOR[i.behavior] || i.behavior)).join("; ") : "—";
        showCards($("dc-study-result"), [
            ["f′(x)", r.firstDerivative],
            ["f″(x)", r.secondDerivative],
            ["Nultočke", r.zeros.length ? r.zeros.map(fmt).join("; ") : "—"],
            ["Ekstremi", r.critical.length ? r.critical.map((c) => c.kind + " (" + fmt(c.x) + ", " + fmt(c.y) + ")").join("; ") : "—"],
            ["Infleksije", r.inflections.length ? r.inflections.map((c) => "(" + fmt(c.x) + ", " + fmt(c.y) + ")").join("; ") : "—"],
            ["Monotonost", ints(r.monotonicity)],
            ["Zakrivljenost", ints(r.concavity)],
            ["Asimptote", r.asymptotes.length ? r.asymptotes.map((x) => x.equation).join("; ") : "—"],
        ]);
        showSteps($("dc-study-steps"), r.steps);
        const canvas = $("dc-study-canvas"), ctx = canvas.getContext("2d");
        const [lo, hi] = rangeY(r.f, 50);
        const v = makeView(canvas, r.xMin, r.xMax, lo, hi);
        drawAxes(ctx, v);
        r.asymptotes.forEach((s) => {
            ctx.strokeStyle = COLORS.axis; ctx.setLineDash([6, 5]); ctx.lineWidth = 1.5;
            ctx.beginPath();
            if (s.kind === "vertical") { ctx.moveTo(v.X(s.x), v.pad); ctx.lineTo(v.X(s.x), v.h - v.pad); }
            else if (s.kind === "horizontal") { ctx.moveTo(v.pad, v.Y(s.offset)); ctx.lineTo(v.w - v.pad, v.Y(s.offset)); }
            else { ctx.moveTo(v.X(v.xMin), v.Y(s.slope * v.xMin + s.offset)); ctx.lineTo(v.X(v.xMax), v.Y(s.slope * v.xMax + s.offset)); }
            ctx.stroke(); ctx.setLineDash([]);
        });
        drawCurve(ctx, v, r.ddf.filter(([, y]) => y === null || Math.abs(y) < 1e3), COLORS.ddf, 1.2);
        drawCurve(ctx, v, r.df.filter(([, y]) => y === null || Math.abs(y) < 1e3), COLORS.df, 1.5);
        drawCurve(ctx, v, r.f, COLORS.f, 2.8);
        r.zeros.forEach((z) => drawPoint(ctx, v, z, 0, COLORS.text, "", true));
        r.critical.forEach((c) => drawPoint(ctx, v, c.x, c.y, COLORS.mark, c.kind === "maximum" ? "max" : c.kind === "minimum" ? "min" : "stac.", false));
        r.inflections.forEach((c) => drawPoint(ctx, v, c.x, c.y, "#7c3aed", "infleksija", true));
    }

    toggles();
    $("dc-limit-calculate").addEventListener("click", runLimit);
    $("dc-cont-calculate").addEventListener("click", runContinuity);
    $("dc-tan-calculate").addEventListener("click", runTangent);
    $("dc-rules-calculate").addEventListener("click", runRules);
    $("dc-mean-calculate").addEventListener("click", runMean);
    $("dc-study-calculate").addEventListener("click", runStudy);
    $("dc-tan-h").addEventListener("input", drawTangent);
}
