"use strict";

// Pretraga tema: filtrira gumbe tema po nazivu i omogucuje brzi skok (otvara poglavlje i panel).
function setupTopicSearch() {
    const input = document.getElementById("topic-search");
    const list = document.getElementById("topic-search-results");
    if (!input || !list) return;

    const norm = (s) => s.toLowerCase().normalize("NFD").replace(/[\u0300-\u036f]/g, "").replace(/đ/g, "d");
    const topics = [...document.querySelectorAll("button[data-panel]")]
        .filter((b) => !b.hidden && b.closest(".chapter") && !b.closest(".chapter").hidden)
        .map((b) => ({
            button: b,
            label: b.textContent.trim(),
            chapter: b.closest(".chapter").querySelector(".chapter-toggle").textContent.replace(/[▸▾]/g, "").trim(),
        }));

    function open(topic) {
        const chapter = topic.button.closest(".chapter");
        const toggle = chapter.querySelector(".chapter-toggle");
        if (toggle.getAttribute("aria-expanded") !== "true") toggle.click();
        const panel = document.getElementById(topic.button.dataset.panel);
        if (panel && panel.hidden) topic.button.click();
        topic.button.scrollIntoView({ behavior: "smooth", block: "center" });
    }

    function render() {
        const q = norm(input.value.trim());
        list.replaceChildren();
        if (q.length < 2) { list.hidden = true; return; }
        const hits = topics.filter((t) => norm(t.label + " " + t.chapter).includes(q)).slice(0, 12);
        if (!hits.length) {
            const li = document.createElement("li");
            li.className = "topic-search-empty";
            li.textContent = "Nema pogodaka.";
            list.append(li);
        }
        hits.forEach((t) => {
            const li = document.createElement("li");
            const b = document.createElement("button");
            b.type = "button";
            b.textContent = `${t.label} — ${t.chapter}`;
            b.addEventListener("click", () => { open(t); list.hidden = true; });
            li.append(b);
            list.append(li);
        });
        list.hidden = false;
    }

    input.addEventListener("input", render);
    input.addEventListener("keydown", (e) => { if (e.key === "Escape") { input.value = ""; render(); } });
}

window.setupTopicSearch = setupTopicSearch;
