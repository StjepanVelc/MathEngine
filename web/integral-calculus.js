"use strict";

// Integralni racun, diferencijalne jednadzbe, nizovi i indukcija: UI sloj. Sva matematika je u C++ jezgri (integral* funkcije).
function setupIntegralCalculus(module) {
    const root = document.querySelector(".integral-calculus-chapter");
    if (!root || typeof module.integralRiemann !== "function") return;

    const $ = (id) => document.getElementById(id);
    const COLORS = { f: "#2563eb", g: "#16a34a", fill: "rgba(234, 88, 12, 0.35)", fillEdge: "#ea580c", axis: "#94a3b8", grid: "#e2e8f0", mark: "#dc2626", text: "#334155" };
    const fmt = (v) => (v === null || v === undefined || !Number.isFinite(v)) ? "—" : new Intl.NumberFormat("hr-HR", { maximumFractionDigits: 6 }).format(v);
    const yesNo = (b) => (b ? "da" : "ne");

    function fail(el, message) {
        const p = document.createElement("p");
        p.className = "error";
        p.textContent = message;
        el.replaceChildren(p);
    }

    function call(fn, resultEl, stepsEl) {
        resultEl.replaceChildren();
        if (stepsEl) stepsEl.replaceChildren();
        let text;
        try { text = fn(); } catch (e) { fail(resultEl, String(e)); return null; }
        if (typeof text === "string" && text.startsWith("GRESKA:")) { fail(resultEl, text.slice(7).trim()); return null; }
        try { return JSON.parse(text); } catch (e) { fail(resultEl, "Neispravan odgovor jezgre."); return null; }
    }

    function showCards(el, pairs) {
        el.replaceChildren(...pairs.map(([label, value]) => {
            const div = document.createElement("div");
            div.className = "result-card";
            const l = document.createElement("span");
            l.textContent = label;
            const v = document.createElement("strong");
            v.textContent = value;
            div.append(l, v);
            return div;
        }));
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

    const num = (id) => Number($(id).value);

    // ---- Canvas pomocnici ----
    function makeView(canvas, xMin, xMax, yMin, yMax) {
        const w = canvas.width, h = canvas.height, pad = 34;
        if (!(xMax > xMin)) xMax = xMin + 1;
        if (!(yMax > yMin)) yMax = yMin + 1;
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

    function drawPoint(ctx, v, x, y, color, label) {
        if (!Number.isFinite(y)) return;
        ctx.beginPath();
        ctx.arc(v.X(x), v.Y(y), 4.5, 0, 2 * Math.PI);
        ctx.fillStyle = color;
        ctx.fill();
        if (label) { ctx.fillStyle = COLORS.text; ctx.font = "11px sans-serif"; ctx.fillText(label, v.X(x) + 7, v.Y(y) - 7); }
    }

    function rangeY(lists) {
        let lo = Infinity, hi = -Infinity;
        for (const pts of lists) for (const [, y] of pts) if (y !== null && Number.isFinite(y) && Math.abs(y) < 1e6) { lo = Math.min(lo, y); hi = Math.max(hi, y); }
        if (!Number.isFinite(lo)) { lo = -1; hi = 1; }
        lo = Math.min(lo, 0); hi = Math.max(hi, 0);
        const m = (hi - lo) * 0.12 || 1;
        return [lo - m, hi + m];
    }

    function sampleCurve(expr, a, b, n) {
        const text = module.integralSample(expr, a, b, n);
        if (typeof text !== "string" || text.startsWith("GRESKA:")) return [];
        return JSON.parse(text).map((p) => [p.x, p.y]);
    }

    function toggles() {
        const buttons = root.querySelectorAll(".ic-toggle");
        const panels = root.querySelectorAll(".ic-panel");
        function show(id) {
            panels.forEach((p) => { p.hidden = p.id !== id; });
            buttons.forEach((b) => { b.classList.toggle("active", b.dataset.panel === id); b.setAttribute("aria-pressed", String(b.dataset.panel === id)); });
        }
        buttons.forEach((b) => b.addEventListener("click", () => show(b.dataset.panel)));
        show("ic-riemann");
    }

    // ---- Riemannove sume ----
    function runRiemann() {
        const expr = $("ic-rie-expression").value, a = num("ic-rie-a"), b = num("ic-rie-b"), n = num("ic-rie-n");
        $("ic-rie-n-label").textContent = String(n);
        const r = call(() => module.integralRiemann(expr, a, b, n, $("ic-rie-method").value), $("ic-rie-result"), $("ic-rie-steps"));
        if (!r) return;
        showCards($("ic-rie-result"), [
            ["Aproksimacija", fmt(r.approximation)], ["Referentna vrijednost", fmt(r.reference)], ["Pogreška", fmt(r.error)],
            ["Donja suma", fmt(r.lowerSum)], ["Gornja suma", fmt(r.upperSum)], ["Širina Δx", fmt(r.stepWidth)],
        ]);
        showSteps($("ic-rie-steps"), r.steps);
        const canvas = $("ic-rie-canvas"), ctx = canvas.getContext("2d");
        const lo = Math.min(a, b), hi = Math.max(a, b), pad = (hi - lo) * 0.08 || 1;
        const pts = sampleCurve(expr, lo - pad, hi + pad, 300);
        const heights = r.rectangles.map((q) => [q.x0, q.height]);
        const [yMin, yMax] = rangeY([pts, heights]);
        const v = makeView(canvas, lo - pad, hi + pad, yMin, yMax);
        drawAxes(ctx, v);
        r.rectangles.forEach((q) => {
            const top = v.Y(q.height), base = v.Y(0);
            ctx.fillStyle = COLORS.fill;
            ctx.strokeStyle = COLORS.fillEdge;
            ctx.lineWidth = 1;
            ctx.fillRect(v.X(q.x0), Math.min(top, base), v.X(q.x1) - v.X(q.x0), Math.abs(base - top));
            ctx.strokeRect(v.X(q.x0), Math.min(top, base), v.X(q.x1) - v.X(q.x0), Math.abs(base - top));
        });
        drawCurve(ctx, v, pts, COLORS.f, 2.6);
    }

    // ---- Neodredeni integral ----
    function runIndefinite() {
        const r = call(() => module.integralIndefinite($("ic-ind-expression").value), $("ic-ind-result"), $("ic-ind-steps"));
        if (!r) return;
        if (!r.found) {
            fail($("ic-ind-result"), r.message || "Antiderivacija nije pronađena.");
            showSteps($("ic-ind-steps"), r.steps || []);
            return;
        }
        showCards($("ic-ind-result"), [["F(x) + C", r.withC], ["Metoda", r.method], ["Provjera F′ = f", yesNo(r.verified)]]);
        showSteps($("ic-ind-steps"), r.steps.concat(r.verification ? [r.verification] : []));
    }

    // ---- Odredeni integral / povrsina ----
    function runDefinite() {
        const f = $("ic-def-f").value, g = $("ic-def-g").value.trim(), a = num("ic-def-a"), b = num("ic-def-b");
        const lo = Math.min(a, b), hi = Math.max(a, b);
        let r;
        if (g) {
            r = call(() => module.integralAreaBetween(f, g, a, b), $("ic-def-result"), $("ic-def-steps"));
            if (!r) return;
            showCards($("ic-def-result"), [["Površina", fmt(r.area)], ["Gornja krivulja", r.upperCurve], ["Donja krivulja", r.lowerCurve], ["Sjecišta", r.crossings.map(fmt).join(", ") || "nema"]]);
        } else {
            r = call(() => module.integralDefinite(f, a, b), $("ic-def-result"), $("ic-def-steps"));
            if (!r) return;
            showCards($("ic-def-result"), [["Vrijednost", fmt(r.value)], ["Metoda", r.method], ["F(x)", r.symbolic ? r.antiderivative : "—"], ["Numerička provjera", fmt(r.numericCheck)], ["Slaganje", yesNo(r.agrees)]]);
        }
        showSteps($("ic-def-steps"), r.steps);
        const canvas = $("ic-def-canvas"), ctx = canvas.getContext("2d");
        const pad = (hi - lo) * 0.1 || 1;
        const pf = sampleCurve(f, lo - pad, hi + pad, 300);
        const pg = g ? sampleCurve(g, lo - pad, hi + pad, 300) : [];
        const [yMin, yMax] = rangeY([pf, pg]);
        const v = makeView(canvas, lo - pad, hi + pad, yMin, yMax);
        drawAxes(ctx, v);
        const fa = sampleCurve(f, lo, hi, 200), ga = g ? sampleCurve(g, lo, hi, 200) : fa.map(([x]) => [x, 0]);
        ctx.fillStyle = COLORS.fill;
        ctx.beginPath();
        fa.forEach(([x, y], i) => { if (i === 0) ctx.moveTo(v.X(x), v.Y(y)); else ctx.lineTo(v.X(x), v.Y(y)); });
        for (let i = ga.length - 1; i >= 0; i--) ctx.lineTo(v.X(ga[i][0]), v.Y(g ? ga[i][1] : 0));
        ctx.closePath();
        ctx.fill();
        drawCurve(ctx, v, pf, COLORS.f, 2.6);
        if (g) drawCurve(ctx, v, pg, COLORS.g, 2.6);
        if (g) r.crossings.forEach((x) => { const s = sampleCurve(f, x, x + 1e-9, 2); if (s.length) drawPoint(ctx, v, x, s[0][1], COLORS.mark, ""); });
    }

    // ---- Nepravi integrali ----
    function runImproper() {
        const kind = $("ic-imp-kind").value;
        const r = call(() => module.integralImproper($("ic-imp-expression").value, kind, num("ic-imp-a"), num("ic-imp-b")), $("ic-imp-result"), $("ic-imp-steps"));
        if (!r) return;
        const labels = { convergent: "konvergira", divergent: "divergira", unknown: "nije moguće odrediti" };
        showCards($("ic-imp-result"), [["Ishod", labels[r.kind] || r.kind], ["Vrijednost", r.kind === "convergent" ? fmt(r.value) : "—"]]);
        showSteps($("ic-imp-steps"), r.steps);
        const canvas = $("ic-imp-canvas"), ctx = canvas.getContext("2d");
        const pts = r.bounds.map((x, i) => [x, r.partials[i]]);
        const xs = r.bounds.length ? r.bounds : [0, 1];
        const [yMin, yMax] = rangeY([pts]);
        const v = makeView(canvas, Math.min(...xs, 0), Math.max(...xs), yMin, yMax);
        drawAxes(ctx, v);
        if (r.kind === "convergent" && Number.isFinite(r.value)) {
            ctx.strokeStyle = COLORS.mark; ctx.setLineDash([6, 5]); ctx.lineWidth = 1.5;
            ctx.beginPath(); ctx.moveTo(v.pad, v.Y(r.value)); ctx.lineTo(v.w - v.pad, v.Y(r.value)); ctx.stroke(); ctx.setLineDash([]);
        }
        drawCurve(ctx, v, pts, COLORS.f, 2.4);
        pts.forEach(([x, y]) => drawPoint(ctx, v, x, y, COLORS.fillEdge, ""));
    }

    // ---- Diferencijalne jednadzbe ----
    function runOde() {
        const kind = $("ic-ode-kind").value;
        const r = call(() => module.integralOde(kind, $("ic-ode-p1").value, $("ic-ode-p2").value, $("ic-ode-initial").checked,
            num("ic-ode-x0"), num("ic-ode-y0"), num("ic-ode-end"), num("ic-ode-k")), $("ic-ode-result"), $("ic-ode-steps"));
        if (!r) return;
        const cards = [["Jednadžba", r.equation], ["Opće rješenje", r.general]];
        if (r.hasInitial) cards.push(["Partikularno rješenje", r.particular], ["Konstanta C", fmt(r.constant)]);
        cards.push(["Provjera", yesNo(r.verified)], ["Najveće odstupanje", fmt(r.residual)]);
        showCards($("ic-ode-result"), cards);
        showSteps($("ic-ode-steps"), r.steps);
        const canvas = $("ic-ode-canvas"), ctx = canvas.getContext("2d");
        const pts = r.xs.map((x, i) => [x, r.ys[i]]);
        if (!pts.length) return;
        const [yMin, yMax] = rangeY([pts]);
        const v = makeView(canvas, Math.min(...r.xs), Math.max(...r.xs), yMin, yMax);
        drawAxes(ctx, v);
        drawCurve(ctx, v, pts, COLORS.f, 2.8);
        if (r.hasInitial) drawPoint(ctx, v, r.x0, r.y0, COLORS.mark, "početni uvjet");
    }

    // ---- Nizovi i indukcija ----
    function runSequence() {
        const r = call(() => module.integralSequence($("ic-seq-expression").value, num("ic-seq-eps"), 30), $("ic-seq-result"), $("ic-seq-steps"));
        if (!r) return;
        const outcome = r.converges ? "konvergira" : r.divergesToInfinity ? "divergira u ∞" : "divergira / ne teži";
        showCards($("ic-seq-result"), [["Ishod", outcome], ["Limes", r.converges ? fmt(r.limit) : "—"], ["Monotonost", r.monotonicity],
            ["Ograničen", yesNo(r.bounded)], ["N za ε", r.epsilonN > 0 ? String(r.epsilonN) : "—"]]);
        showSteps($("ic-seq-steps"), r.steps);
        const canvas = $("ic-seq-canvas"), ctx = canvas.getContext("2d");
        const pts = r.terms.map((y, i) => [i + 1, y]);
        const [yMin, yMax] = rangeY([pts]);
        const v = makeView(canvas, 0, Math.max(r.terms.length + 1, 2), yMin, yMax);
        drawAxes(ctx, v);
        if (r.converges && Number.isFinite(r.limit)) {
            ctx.fillStyle = "rgba(220, 38, 38, 0.12)";
            const e = r.epsilon;
            ctx.fillRect(v.pad, v.Y(r.limit + e), v.w - 2 * v.pad, v.Y(r.limit - e) - v.Y(r.limit + e));
            ctx.strokeStyle = COLORS.mark; ctx.setLineDash([6, 5]); ctx.lineWidth = 1.5;
            ctx.beginPath(); ctx.moveTo(v.pad, v.Y(r.limit)); ctx.lineTo(v.w - v.pad, v.Y(r.limit)); ctx.stroke(); ctx.setLineDash([]);
        }
        pts.forEach(([x, y]) => drawPoint(ctx, v, x, y, COLORS.f, ""));
    }

    function runInduction() {
        const r = call(() => module.integralInduction($("ic-idx-kind").value, $("ic-idx-left").value, $("ic-idx-right").value, num("ic-idx-n0"), 60), $("ic-idx-result"), $("ic-idx-steps"));
        if (!r) return;
        showCards($("ic-idx-result"), [["Tvrdnja", r.statement], ["Baza", yesNo(r.baseCaseHolds)], ["Provjereno za n", String(r.checked)],
            ["Korak potvrđen", yesNo(r.stepVerified)], ["Prvi protuprimjer", r.firstCounterexample ? String(r.firstCounterexample) : "nema"]]);
        showSteps($("ic-idx-steps"), r.steps);
    }

    toggles();
    $("ic-rie-calculate").addEventListener("click", runRiemann);
    $("ic-rie-n").addEventListener("input", runRiemann);
    $("ic-ind-calculate").addEventListener("click", runIndefinite);
    $("ic-def-calculate").addEventListener("click", runDefinite);
    $("ic-imp-calculate").addEventListener("click", runImproper);
    $("ic-ode-calculate").addEventListener("click", runOde);
    $("ic-seq-calculate").addEventListener("click", runSequence);
    $("ic-idx-calculate").addEventListener("click", runInduction);
}
