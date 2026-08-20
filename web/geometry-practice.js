function setupGeometryPractice(module, helpers) {
    const root = document.getElementById("geometry-practice-root");
    const storageKey = "mathengine-geometry-practice-v1";
    const levels = { lagano: "Lagano", srednje: "Srednje", izazov: "Izazov" };
    let bank = null;
    let difficulty = "lagano";
    let current = null;
    let previousId = null;
    let state = loadState();

    root.innerHTML = `
        <div class="practice-heading">
            <div><span class="lesson-chip">Uči kroz igru</span><h2>Geometrijska vježbaonica</h2><p>Odaberi izazov, riješi zadatak i izgradi niz točnih odgovora.</p></div>
            <div class="practice-score"><span>Bodovi</span><strong id="geometry-practice-points">0</strong></div>
        </div>
        <div class="practice-dashboard">
            <div><span>Riješeno</span><strong id="geometry-practice-solved">0</strong></div>
            <div><span>Točnost</span><strong id="geometry-practice-accuracy">—</strong></div>
            <div><span>Najbolji niz</span><strong id="geometry-practice-best">0</strong></div>
        </div>
        <div class="practice-difficulties" role="group" aria-label="Težina zadatka">
            <button type="button" data-difficulty="lagano" class="active">Lagano</button>
            <button type="button" data-difficulty="srednje">Srednje</button>
            <button type="button" data-difficulty="izazov">Izazov</button>
        </div>
        <article class="practice-card">
            <div class="practice-card-top"><span id="geometry-practice-topic" class="practice-topic">Geometrija</span><span id="geometry-practice-counter"></span></div>
            <h3 id="geometry-practice-title">Učitavam zadatke…</h3>
            <p id="geometry-practice-question" class="practice-question"></p>
            <div class="practice-answer-row"><input id="geometry-practice-answer" type="text" inputmode="decimal" autocomplete="off" placeholder="Upiši odgovor"><span id="geometry-practice-unit" class="practice-unit"></span></div>
            <div class="practice-actions"><button type="button" id="geometry-practice-check">Provjeri</button><button type="button" id="geometry-practice-hint" class="secondary">Mali trag</button><button type="button" id="geometry-practice-next" class="secondary">Novi zadatak</button></div>
            <p id="geometry-practice-feedback" class="practice-feedback" aria-live="polite"></p>
            <div id="geometry-practice-hint-box" class="practice-hint" hidden></div>
            <div id="geometry-practice-solution" class="practice-solution" hidden></div>
        </article>
        <button type="button" id="geometry-practice-reset" class="practice-reset">Poništi moj napredak</button>`;

    function loadState() {
        try {
            return { attempts: 0, correct: 0, streak: 0, best: 0, points: 0, completed: {}, ...JSON.parse(localStorage.getItem(storageKey) || "{}") };
        } catch (_) {
            return { attempts: 0, correct: 0, streak: 0, best: 0, points: 0, completed: {} };
        }
    }

    function saveState() {
        localStorage.setItem(storageKey, JSON.stringify(state));
        updateDashboard();
    }

    function updateDashboard() {
        document.getElementById("geometry-practice-points").textContent = state.points;
        document.getElementById("geometry-practice-solved").textContent = state.correct;
        document.getElementById("geometry-practice-accuracy").textContent = state.attempts ? `${Math.round(state.correct / state.attempts * 100)}%` : "—";
        document.getElementById("geometry-practice-best").textContent = state.best;
    }

    function chooseExercise() {
        if (!bank) return;
        const exercises = bank[difficulty];
        let candidates = exercises.filter((exercise) => exercise.id !== previousId);
        if (!candidates.length) candidates = exercises;
        const leastSolved = Math.min(...candidates.map((exercise) => state.completed[exercise.id] || 0));
        const pool = candidates.filter((exercise) => (state.completed[exercise.id] || 0) === leastSolved);
        current = pool[Math.floor(Math.random() * pool.length)];
        previousId = current.id;
        renderExercise();
    }

    function renderExercise() {
        document.getElementById("geometry-practice-topic").textContent = current.topic;
        document.getElementById("geometry-practice-title").textContent = current.title;
        document.getElementById("geometry-practice-question").textContent = current.question;
        document.getElementById("geometry-practice-unit").textContent = current.unit;
        document.getElementById("geometry-practice-counter").textContent = `${levels[difficulty]} · niz ${state.streak}`;
        document.getElementById("geometry-practice-answer").value = "";
        document.getElementById("geometry-practice-feedback").textContent = "";
        document.getElementById("geometry-practice-feedback").className = "practice-feedback";
        document.getElementById("geometry-practice-hint-box").hidden = true;
        document.getElementById("geometry-practice-solution").hidden = true;
        document.getElementById("geometry-practice-check").disabled = false;
        document.getElementById("geometry-practice-answer").focus({ preventScroll: true });
    }

    function readAnswer() {
        const text = document.getElementById("geometry-practice-answer").value.trim().replace(",", ".");
        if (!text) throw new Error("Najprije upiši odgovor.");
        const value = Number(text);
        if (!Number.isFinite(value)) throw new Error("Odgovor mora biti broj.");
        return value;
    }

    function check() {
        if (!current) return;
        const feedback = document.getElementById("geometry-practice-feedback");
        try {
            const answer = readAnswer();
            const correct = Math.abs(answer - current.answer) <= current.tolerance;
            state.attempts++;
            if (correct) {
                state.correct++;
                state.streak++;
                state.best = Math.max(state.best, state.streak);
                const multiplier = difficulty === "lagano" ? 10 : difficulty === "srednje" ? 20 : 30;
                state.points += multiplier + Math.min(state.streak - 1, 5) * 2;
                state.completed[current.id] = (state.completed[current.id] || 0) + 1;
                feedback.textContent = `Bravo! Točan odgovor je ${helpers.format(current.answer)} ${current.unit}.`;
                feedback.classList.add("correct");
            } else {
                state.streak = 0;
                feedback.textContent = "Još nije točno. Pogledaj mali trag ili pokušaj ponovno na sljedećem zadatku.";
                feedback.classList.add("incorrect");
            }
            document.getElementById("geometry-practice-solution").innerHTML = `<strong>Kako dolazimo do rješenja?</strong><p>${current.explanation}</p>`;
            document.getElementById("geometry-practice-solution").hidden = false;
            document.getElementById("geometry-practice-check").disabled = true;
            saveState();
        } catch (error) {
            feedback.textContent = error.message;
            feedback.classList.add("incorrect");
        }
    }

    root.querySelectorAll("[data-difficulty]").forEach((button) => button.addEventListener("click", () => {
        difficulty = button.dataset.difficulty;
        root.querySelectorAll("[data-difficulty]").forEach((item) => item.classList.toggle("active", item === button));
        chooseExercise();
    }));
    document.getElementById("geometry-practice-check").addEventListener("click", check);
    document.getElementById("geometry-practice-answer").addEventListener("keydown", (event) => { if (event.key === "Enter") check(); });
    document.getElementById("geometry-practice-next").addEventListener("click", chooseExercise);
    document.getElementById("geometry-practice-hint").addEventListener("click", () => {
        if (!current) return;
        const box = document.getElementById("geometry-practice-hint-box");
        box.textContent = current.hint;
        box.hidden = false;
    });
    document.getElementById("geometry-practice-reset").addEventListener("click", () => {
        state = { attempts: 0, correct: 0, streak: 0, best: 0, points: 0, completed: {} };
        saveState();
        chooseExercise();
    });

    updateDashboard();
    fetch("data/geometry-exercises.json")
        .then((response) => {
            if (!response.ok) throw new Error("Banka zadataka nije dostupna.");
            return response.json();
        })
        .then((data) => { bank = data; chooseExercise(); })
        .catch((error) => {
            document.getElementById("geometry-practice-title").textContent = "Zadatke nije moguće učitati";
            document.getElementById("geometry-practice-question").textContent = error.message;
        });

    void module;
}
