// Interaktivni Canvas renderer za polinomske funkcije.

function setupAlgebraGraph(module) {
    const canvas = document.getElementById("algebra-graph");
    const context = canvas.getContext("2d");
    const expressionInput = document.getElementById("graph-expression");
    const coordinateOutput = document.getElementById("graph-coordinates");
    const analysisBox = document.getElementById("graph-analysis");
    const reducedMotion = matchMedia("(prefers-reduced-motion: reduce)").matches;
    const colors = { grid: "#45475a", axis: "#bac2de", curve: "#89b4fa", point: "#f9e2af", text: "#cdd6f4" };
    let centerX = 0;
    let centerY = 0;
    let unitsPerWidth = 20;
    let current = null;
    let previousSamples = null;
    let animationStart = 0;
    let dragging = false;
    let dragPoint = null;

    function substituteParameters(expression) {
        const values = {
            a: document.getElementById("graph-a").value,
            b: document.getElementById("graph-b").value,
            c: document.getElementById("graph-c").value
        };
        return expression.replace(/\b([abc])\b/g, (_, name) => `(${values[name]})`);
    }

    function bounds() {
        const aspect = canvas.height / canvas.width;
        const halfX = unitsPerWidth / 2;
        return { minX: centerX - halfX, maxX: centerX + halfX, minY: centerY - halfX * aspect, maxY: centerY + halfX * aspect };
    }

    function toCanvas(x, y) {
        const view = bounds();
        return [(x - view.minX) / (view.maxX - view.minX) * canvas.width,
            canvas.height - (y - view.minY) / (view.maxY - view.minY) * canvas.height];
    }

    function toWorld(px, py) {
        const view = bounds();
        return [view.minX + px / canvas.width * (view.maxX - view.minX),
            view.maxY - py / canvas.height * (view.maxY - view.minY)];
    }

    function niceStep(range) {
        const rough = range / 10;
        const power = 10 ** Math.floor(Math.log10(rough));
        const normalized = rough / power;
        return (normalized < 2 ? 1 : normalized < 5 ? 2 : 5) * power;
    }

    function drawGrid() {
        const view = bounds();
        const step = niceStep(view.maxX - view.minX);
        context.lineWidth = 1;
        context.font = "12px system-ui";
        for (let x = Math.ceil(view.minX / step) * step; x <= view.maxX; x += step) {
            const [px] = toCanvas(x, 0);
            context.strokeStyle = Math.abs(x) < step / 100 ? colors.axis : colors.grid;
            context.beginPath(); context.moveTo(px, 0); context.lineTo(px, canvas.height); context.stroke();
            if (Math.abs(x) >= step / 100) { context.fillStyle = colors.text; context.fillText(formatNumberForDisplay(x, 4), px + 4, Math.min(canvas.height - 5, Math.max(14, toCanvas(0, 0)[1] - 5))); }
        }
        for (let y = Math.ceil(view.minY / step) * step; y <= view.maxY; y += step) {
            const [, py] = toCanvas(0, y);
            context.strokeStyle = Math.abs(y) < step / 100 ? colors.axis : colors.grid;
            context.beginPath(); context.moveTo(0, py); context.lineTo(canvas.width, py); context.stroke();
            if (Math.abs(y) >= step / 100) { context.fillStyle = colors.text; context.fillText(formatNumberForDisplay(y, 4), Math.min(canvas.width - 45, Math.max(5, toCanvas(0, 0)[0] + 5)), py - 4); }
        }
    }

    function interpolatedSamples(progress) {
        if (!previousSamples || previousSamples.length !== current.samples.length) return current.samples;
        return current.samples.map((point, index) => [point[0], previousSamples[index][1] + (point[1] - previousSamples[index][1]) * progress]);
    }

    function drawCurve(samples) {
        const view = bounds();
        context.strokeStyle = colors.curve;
        context.lineWidth = 3;
        context.beginPath();
        let started = false;
        let previousY = 0;
        for (const [x, y] of samples) {
            const [px, py] = toCanvas(x, y);
            const discontinuity = !Number.isFinite(py) || (started && Math.abs(py - previousY) > canvas.height * 1.5);
            if (discontinuity || x < view.minX || x > view.maxX) { started = false; continue; }
            if (!started) { context.moveTo(px, py); started = true; } else context.lineTo(px, py);
            previousY = py;
        }
        context.stroke();
    }

    function drawPoint(point, label) {
        if (!point) return;
        const [x, y] = toCanvas(point.x, point.y);
        if (x < 0 || x > canvas.width || y < 0 || y > canvas.height) return;
        context.fillStyle = colors.point;
        context.beginPath(); context.arc(x, y, 5, 0, Math.PI * 2); context.fill();
        context.fillStyle = colors.text;
        context.fillText(`${label} (${formatNumberForDisplay(point.x)}, ${formatNumberForDisplay(point.y)})`, x + 8, y - 8);
    }

    function paint(timestamp = performance.now()) {
        context.clearRect(0, 0, canvas.width, canvas.height);
        drawGrid();
        if (!current) return;
        const progress = reducedMotion ? 1 : Math.min(1, (timestamp - animationStart) / 350);
        drawCurve(interpolatedSamples(1 - (1 - progress) ** 3));
        if (progress === 1) {
            current.xIntercepts.forEach((point) => drawPoint(point, "Nultočka"));
            drawPoint(current.yIntercept, "Sjecište s y-osi:");
            drawPoint(current.vertex, "Vrh");
        } else requestAnimationFrame(paint);
    }

    function renderAnalysis(data) {
        analysisBox.textContent = `f(x) = ${data.normalized.replace(/\^2/g, "²")} | Domena: ${data.domain} | ${data.behavior}`;
    }

    function analyze(animate = true) {
        const expression = substituteParameters(expressionInput.value.trim());
        const view = bounds();
        const text = module.algebraAnalyzeFunction(expression, view.minX, view.maxX, Math.max(300, Math.floor(canvas.width)));
        if (text.startsWith("GRESKA:")) {
            analysisBox.textContent = text;
            analysisBox.classList.add("error");
            return;
        }
        analysisBox.classList.remove("error");
        previousSamples = animate && current ? current.samples : null;
        current = JSON.parse(text);
        animationStart = performance.now();
        renderAnalysis(current);
        requestAnimationFrame(paint);
    }

    function resize() {
        const rectangle = canvas.getBoundingClientRect();
        if (!rectangle.width) return;
        canvas.width = Math.floor(rectangle.width);
        canvas.height = Math.floor(rectangle.width * 0.62);
        analyze(false);
    }

    document.getElementById("graph-draw").addEventListener("click", () => analyze());
    document.getElementById("graph-reset-view").addEventListener("click", () => { centerX = 0; centerY = 0; unitsPerWidth = 20; analyze(false); });
    ["a", "b", "c"].forEach((name) => {
        const input = document.getElementById(`graph-${name}`);
        const output = document.getElementById(`graph-${name}-value`);
        input.addEventListener("input", () => { output.textContent = input.value; analyze(); });
    });
    canvas.addEventListener("wheel", (event) => {
        event.preventDefault();
        unitsPerWidth = Math.min(200, Math.max(2, unitsPerWidth * Math.exp(event.deltaY * 0.001)));
        analyze(false);
    }, { passive: false });
    canvas.addEventListener("pointerdown", (event) => { dragging = true; dragPoint = [event.clientX, event.clientY]; canvas.setPointerCapture(event.pointerId); });
    canvas.addEventListener("pointermove", (event) => {
        const rectangle = canvas.getBoundingClientRect();
        const [x, y] = toWorld((event.clientX - rectangle.left) * canvas.width / rectangle.width,
            (event.clientY - rectangle.top) * canvas.height / rectangle.height);
        coordinateOutput.textContent = `x: ${formatNumberForDisplay(x)}, y: ${formatNumberForDisplay(y)}`;
        if (!dragging) return;
        const scale = unitsPerWidth / rectangle.width;
        centerX -= (event.clientX - dragPoint[0]) * scale;
        centerY += (event.clientY - dragPoint[1]) * scale;
        dragPoint = [event.clientX, event.clientY];
        analyze(false);
    });
    canvas.addEventListener("pointerup", () => { dragging = false; });
    canvas.addEventListener("pointerleave", () => { if (!dragging) coordinateOutput.textContent = "x: —, y: —"; });
    window.addEventListener("resize", resize);
    resize();
}
