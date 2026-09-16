// Zajednička zaštita unosa i formatiranje brojeva za sve alate na razini-stranicama.
//
// 1) guardInputs() sprječava unos znakova '<' i '>' u bilo koje tekstualno/brojčano
//    polje (ti znakovi nisu potrebni ni u jednom matematičkom/logičkom unosu, a
//    upravo preko njih ide klasičan pokušaj ubacivanja HTML/skripti) te ograničava
//    duljinu unosa kako WASM parser ne bi dobio apsurdno dugačak string.
//    Radi generički (event delegation na document razini) pa pokriva i inpute koje
//    moduli generiraju dinamički nakon učitavanja stranice.
//
// 2) formatNumberForDisplay(n) prikazuje vrlo velike ili vrlo male brojeve u
//    znanstvenom zapisu (npr. 1.234e+8) umjesto pune decimale, radi čitljivosti na
//    grafovima i u ispisima. Ne mijenja internu preciznost izračuna, samo prikaz.

(function () {
    const MAX_TEXT_LENGTH = 200;
    const MAX_NUMBER_LENGTH = 32;

    function isGuardedInput(el) {
        if (!el || el.tagName !== "INPUT") return false;
        const type = (el.getAttribute("type") || "text").toLowerCase();
        return type === "text" || type === "number" || type === "search";
    }

    function guardValue(el) {
        if (!isGuardedInput(el)) return;
        const original = el.value;
        let value = original.replace(/[<>]/g, "");
        const max = el.type === "number" ? MAX_NUMBER_LENGTH : MAX_TEXT_LENGTH;
        if (value.length > max) value = value.slice(0, max);
        if (value !== original) el.value = value;
    }

    document.addEventListener("input", (event) => guardValue(event.target), true);
    document.addEventListener("paste", () => {
        // Vrijednost se ažurira tek nakon paste eventa; provjeri na sljedećem tick-u.
        setTimeout(() => guardValue(document.activeElement), 0);
    }, true);

    window.guardValue = guardValue;
})();

function formatNumberForDisplay(value, digits = 3) {
    const n = typeof value === "number" ? value : Number(value);
    if (!Number.isFinite(n)) return String(value);
    if (n === 0) return "0";
    const abs = Math.abs(n);
    if (abs >= 1e6 || abs < 1e-6) return n.toExponential(digits);
    return Number(n.toFixed(digits)).toString();
}
