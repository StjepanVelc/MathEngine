// Formalizacija rečenica — interaktivne vježbe po težinama.
// Korisnikova formula se prihvaća ako je logički ekvivalentna rješenju
// (WASM izvoz: logicEquivalent).

function setupFormalization(module) {
    const exerciseBox = document.getElementById("form-exercise");
    const sentenceEl = document.getElementById("form-sentence");
    const legendEl = document.getElementById("form-legend");
    const input = document.getElementById("form-input");
    const checkBtn = document.getElementById("form-check");
    const nextBtn = document.getElementById("form-next");
    const feedback = document.getElementById("form-feedback");
    const explanation = document.getElementById("form-explanation");
    const progress = document.getElementById("form-progress");
    const resetCurrentBtn = document.getElementById("form-reset-current");
    const resetAllBtn = document.getElementById("form-reset-all");
    const difficultyButtons = document.querySelectorAll(".difficulty-btn");

    let currentDifficulty = null;
    let exercises = null;
    let currentExercise = null;
    let previousId = null;
    let attemptRecorded = false;
    const stats = JSON.parse(localStorage.getItem("mathengine.formalization.progress") || "{}");

    setupPalette(document.querySelector('.symbol-palette[data-target="form-input"]'));

    function saveStats() {
        localStorage.setItem("mathengine.formalization.progress", JSON.stringify(stats));
    }

    function updateProgress() {
        const value = stats[currentDifficulty] || { correct: 0, attempts: 0 };
        progress.textContent = currentDifficulty
            ? `Napredak: ${value.correct} točnih od ${value.attempts} pokušaja.`
            : "";
    }

    function chooseExercise() {
        const pool = exercises[currentDifficulty];
        const available = pool.length > 1 ? pool.filter((item) => item.id !== previousId) : pool;
        return available[Math.floor(Math.random() * available.length)];
    }

    function showExercise() {
        currentExercise = chooseExercise();
        previousId = currentExercise.id;
        const exercise = currentExercise;
        sentenceEl.textContent = "„" + exercise.sentence + "”";
        legendEl.textContent = exercise.legend;
        input.value = "";
        feedback.textContent = "";
        feedback.className = "parsed";
        explanation.textContent = "";
        explanation.hidden = true;
        attemptRecorded = false;
        exerciseBox.hidden = false;
        input.focus();
    }

    function check() {
        const exercise = currentExercise;
        const answer = input.value.trim();
        if (!answer) return;
        const text = module.logicEquivalent(answer, exercise.solution);
        if (text.startsWith("GRESKA:")) {
            feedback.textContent = text;
            feedback.className = "parsed error";
        } else if (text === "T") {
            feedback.textContent = "✔ Točno! Formula je ekvivalentna rješenju.";
            feedback.className = "parsed correct";
            if (!attemptRecorded) {
                stats[currentDifficulty] ||= { correct: 0, attempts: 0 };
                stats[currentDifficulty].correct += 1;
                stats[currentDifficulty].attempts += 1;
                attemptRecorded = true;
            }
            explanation.textContent = exercise.explanation;
            explanation.hidden = false;
            saveStats();
            updateProgress();
        } else {
            feedback.textContent = "✘ Nije ekvivalentno — pokušaj ponovno. (Pazi na imena varijabli iz legende.)";
            feedback.className = "parsed error";
            if (!attemptRecorded) {
                stats[currentDifficulty] ||= { correct: 0, attempts: 0 };
                stats[currentDifficulty].attempts += 1;
                attemptRecorded = true;
                saveStats();
                updateProgress();
            }
        }
    }

    function next() {
        showExercise();
    }

    difficultyButtons.forEach((btn) => {
        btn.addEventListener("click", async () => {
            if (!exercises) {
                try {
                    const response = await fetch("data/formalization-exercises.json");
                    if (!response.ok) throw new Error(`HTTP ${response.status}`);
                    exercises = await response.json();
                    for (const difficulty of ["pocetna", "srednja", "napredna"]) {
                        if (!Array.isArray(exercises[difficulty]) || exercises[difficulty].length === 0) {
                            throw new Error(`Nedostaju zadaci za težinu '${difficulty}'`);
                        }
                        for (const exercise of exercises[difficulty]) {
                            if (!exercise.id || !exercise.sentence || !exercise.legend ||
                                !exercise.solution || !exercise.explanation) {
                                throw new Error(`Neispravan zadatak u težini '${difficulty}'`);
                            }
                        }
                    }
                } catch (error) {
                    feedback.textContent = "GRESKA: Zadaci se ne mogu učitati. Pokreni stranicu preko lokalnog HTTP servera.";
                    feedback.className = "parsed error";
                    exerciseBox.hidden = false;
                    return;
                }
            }
            difficultyButtons.forEach((b) => b.classList.remove("active"));
            btn.classList.add("active");
            currentDifficulty = btn.dataset.difficulty;
            previousId = null;
            updateProgress();
            showExercise();
        });
    });

    checkBtn.addEventListener("click", check);
    nextBtn.addEventListener("click", next);
    resetCurrentBtn.addEventListener("click", () => {
        if (!currentDifficulty) return;
        delete stats[currentDifficulty];
        saveStats();
        updateProgress();
    });
    resetAllBtn.addEventListener("click", () => {
        for (const key of Object.keys(stats)) delete stats[key];
        saveStats();
        updateProgress();
    });
    input.addEventListener("keydown", (e) => {
        if (e.key === "Enter") check();
    });
}
