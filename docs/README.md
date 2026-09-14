# MathEngine dokumentacija

Ova mapa sadrži tehničku dokumentaciju aktualne implementacije MathEnginea. Dokumenti su namijenjeni razvoju, održavanju, testiranju i uvođenju novih matematičkih domena.

## Brza navigacija

| Dokument | Sadržaj |
|---|---|
| [Arhitektura](architecture.md) | Slojevi sustava, tok podataka, struktura repozitorija i dizajnerske odluke |
| [C++ matematičke domene](core-domains.md) | Aritmetika, algebra, logika, geometrija, trigonometrija te nizovi i redovi |
| [WebAssembly API](wasm-api.md) | Embind izvozi, JSON ugovori, greške i generirani artefakti |
| [Web frontend](web-frontend.md) | Bootstrap, obrazovne razine, UI moduli, grafovi i SVG vizualizacije |
| [Zadaci i lokalna pohrana](exercises-and-storage.md) | JSON banke, provjera odgovora, napredak i `localStorage` |
| [Build i pokretanje](build-and-run.md) | CMake preseti, native/WASM build i lokalni web server |
| [Testiranje i CI](testing-and-ci.md) | GoogleTest, smoke testovi, web validacija i GitHub Actions |
| [Proširivanje sustava](extending-mathengine.md) | Postupak dodavanja nove domene, API-ja, UI-ja i testova |
| [Obrazovni roadmap](roadmap.md) | Redoslijed razvoja, statusi te odvajanje srednjoškolskih i fakultetskih modula |

## Status domena

### Aktivne domene

- `arithmetic`
- `algebra`
- `logic`
- `predicate`
- `geometry`
- `trigonometry`
- `sequences`
- `analytic_geometry`
- `exponential_logarithmic`
- `combinatorics_probability_statistics`
- `calculus_basics`
- `mathematical_analysis`
- `linear_algebra`
- `discrete_math`
- `probability_statistics`
- `complex_numbers`

Aktivne domene imaju javna C++ zaglavlja, implementacije, CMake registraciju, testove i prema potrebi WASM adaptere. `linear_algebra`, `discrete_math`, `probability_statistics` i `complex_numbers` su fakultetska proširenja: imaju vlastite WASM adaptere (`linearAlgebra*`, `discreteMath*`, `probabilityStatistics*`, `complexNumbers*` u `core/src/wasm_bindings.cpp`) i frontend module (`web/analytic-algebra.js`, `web/discrete-math.js`, `web/probability-statistics.js`, `web/complex-numbers.js`).

### Planirane domene

Trenutačno nema domena bez implementacije. Fakultetsko proširenje matematičke analize (`mathematical_analysis`) je implementirano i aktivno, odvojeno od srednjoškolskog modula `calculus_basics`. Fakultetsko proširenje vjerojatnosti/statistike (`probability_statistics`) je implementirano i aktivno, odvojeno od već implementiranog srednjoškolskog modula `combinatorics_probability_statistics`.

## Konvencije dokumentacije

- Putanje su relativne u odnosu na korijen repozitorija.
- C++ simboli i JavaScript funkcije zapisani su izvornim imenima.
- Primjeri JSON-a predstavljaju ugovor između WASM sloja i frontenda.
- `web/aksiomat.js` i `web/aksiomat.wasm` generirani su build artefakti; ne uređuju se ručno.
- Broj testova naveden u dokumentaciji predstavlja posljednje validirano stanje i može rasti.
