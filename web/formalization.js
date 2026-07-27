// Formalizacija rečenica — interaktivne vježbe po težinama.
// Korisnikova formula se prihvaća ako je logički ekvivalentna rješenju
// (WASM izvoz: logicEquivalent).

const FORMALIZATION_EXERCISES = {
    pocetna: [
        {
            sentence: "Pada kiša i puše vjetar.",
            legend: "k = pada kiša, v = puše vjetar",
            solution: "k & v",
        },
        {
            sentence: "Ne pada kiša.",
            legend: "k = pada kiša",
            solution: "!k",
        },
        {
            sentence: "Učit ću ili ću spavati.",
            legend: "u = učit ću, s = spavat ću",
            solution: "u | s",
        },
        {
            sentence: "Ako pada kiša, onda je cesta mokra.",
            legend: "k = pada kiša, m = cesta je mokra",
            solution: "k -> m",
        },
    ],
    srednja: [
        {
            sentence: "Ako pada kiša i nemam kišobran, onda ću pokisnuti.",
            legend: "k = pada kiša, o = imam kišobran, p = pokisnut ću",
            solution: "(k & !o) -> p",
        },
        {
            sentence: "Idem na izlet ako i samo ako je sunčano.",
            legend: "i = idem na izlet, s = sunčano je",
            solution: "i <-> s",
        },
        {
            sentence: "Nije istina da su i Ana i Marko položili ispit.",
            legend: "a = Ana je položila, m = Marko je položio",
            solution: "!(a & m)",
        },
        {
            sentence: "Ako ne učim, neću položiti ispit.",
            legend: "u = učim, p = položit ću ispit",
            solution: "!u -> !p",
        },
    ],
    napredna: [
        {
            sentence: "Ako je sustav preopterećen ili je nestalo struje, server ne radi, a ako server ne radi, korisnici se žale.",
            legend: "p = sustav je preopterećen, s = nestalo je struje, r = server radi, z = korisnici se žale",
            solution: "((p | s) -> !r) & (!r -> z)",
        },
        {
            sentence: "Doći ću na sastanak samo ako završim posao, a ako dođem, neću ostati dugo.",
            legend: "d = doći ću na sastanak, z = završit ću posao, o = ostat ću dugo",
            solution: "(d -> z) & (d -> !o)",
        },
        {
            sentence: "Barem jedno od dvoje: ili je lift pokvaren ili se penjemo stepenicama, ali nikako oboje.",
            legend: "l = lift je pokvaren, s = penjemo se stepenicama",
            solution: "(l | s) & !(l & s)",
        },
    ],
};

function setupFormalization(module) {
    const exerciseBox = document.getElementById("form-exercise");
    const sentenceEl = document.getElementById("form-sentence");
    const legendEl = document.getElementById("form-legend");
    const input = document.getElementById("form-input");
    const checkBtn = document.getElementById("form-check");
    const nextBtn = document.getElementById("form-next");
    const feedback = document.getElementById("form-feedback");
    const difficultyButtons = document.querySelectorAll(".difficulty-btn");

    let currentDifficulty = null;
    let currentIndex = 0;

    setupPalette(document.querySelector('.symbol-palette[data-target="form-input"]'));

    function showExercise() {
        const exercise = FORMALIZATION_EXERCISES[currentDifficulty][currentIndex];
        sentenceEl.textContent = "„" + exercise.sentence + "”";
        legendEl.textContent = exercise.legend;
        input.value = "";
        feedback.textContent = "";
        feedback.className = "parsed";
        exerciseBox.hidden = false;
        input.focus();
    }

    function check() {
        const exercise = FORMALIZATION_EXERCISES[currentDifficulty][currentIndex];
        const answer = input.value.trim();
        if (!answer) return;
        const text = module.logicEquivalent(answer, exercise.solution);
        if (text.startsWith("GRESKA:")) {
            feedback.textContent = text;
            feedback.className = "parsed error";
        } else if (text === "T") {
            feedback.textContent = "✔ Točno! Formula je ekvivalentna rješenju.";
            feedback.className = "parsed correct";
        } else {
            feedback.textContent = "✘ Nije ekvivalentno — pokušaj ponovno. (Pazi na imena varijabli iz legende.)";
            feedback.className = "parsed error";
        }
    }

    function next() {
        const pool = FORMALIZATION_EXERCISES[currentDifficulty];
        currentIndex = (currentIndex + 1) % pool.length;
        showExercise();
    }

    difficultyButtons.forEach((btn) => {
        btn.addEventListener("click", () => {
            difficultyButtons.forEach((b) => b.classList.remove("active"));
            btn.classList.add("active");
            currentDifficulty = btn.dataset.difficulty;
            currentIndex = 0;
            showExercise();
        });
    });

    checkBtn.addEventListener("click", check);
    nextBtn.addEventListener("click", next);
    input.addEventListener("keydown", (e) => {
        if (e.key === "Enter") check();
    });
}
