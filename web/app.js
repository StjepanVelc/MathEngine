// MathEngine web frontend — landing stranica (izbor razine ucenja).
// Nakon odabira razine, korisnik se preusmjerava na posebnu stranicu
// (osnovna-skola.html / srednja-skola.html / fakultet.html) koja sadrzi
// alate i poglavlja prilagodena toj razini.

const learningLevels = {
    primary: { label: "Osnovna skola", page: "osnovna-skola.html" },
    secondary: { label: "Srednja skola", page: "srednja-skola.html" },
    advanced: { label: "Napredno i fakultet", page: "fakultet.html" }
};

function setupLearningLevels() {
    const section = document.getElementById("learning-levels");
    const cards = [...document.querySelectorAll(".level-card")];
    const activeLabel = document.getElementById("active-level-label");
    const changeButton = document.getElementById("change-learning-level");
    const currentLevel = document.getElementById("current-learning-level");
    const enterButton = document.getElementById("learning-levels-enter");
    const levelGrid = document.getElementById("learning-levels-grid");

    function openLevelGrid() {
        levelGrid.hidden = false;
        enterButton.hidden = true;
    }

    function closeLevelGrid() {
        levelGrid.hidden = true;
        enterButton.hidden = false;
    }

    function showLevelSelector() {
        section.hidden = false;
        changeButton.hidden = true;
        currentLevel.hidden = true;
        closeLevelGrid();
        const savedLevel = localStorage.getItem("mathengine-learning-level");
        activeLabel.textContent = learningLevels[savedLevel]?.label || "Nije odabrano";
        cards.forEach((card) => {
            const active = card.dataset.level === savedLevel;
            card.classList.toggle("active", active);
            card.setAttribute("aria-pressed", String(active));
        });
        section.scrollIntoView({ behavior: "smooth", block: "start" });
    }

    function selectLevel(level) {
        const entry = learningLevels[level];
        if (!entry) return;
        localStorage.setItem("mathengine-learning-level", level);
        window.location.href = entry.page;
    }

    function resetLevelSelection() {
        localStorage.removeItem("mathengine-learning-level");
        showLevelSelector();
        window.scrollTo({ top: 0, behavior: "smooth" });
    }

    cards.forEach((card) => card.addEventListener("click", () => selectLevel(card.dataset.level)));
    changeButton.addEventListener("click", showLevelSelector);
    enterButton.addEventListener("click", openLevelGrid);

    const homeLink = document.getElementById("nav-home");
    if (homeLink) {
        homeLink.addEventListener("click", (event) => {
            event.preventDefault();
            resetLevelSelection();
        });
    }

    showLevelSelector();
}

setupLearningLevels();
