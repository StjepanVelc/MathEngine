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

## Status domena

### Aktivne domene

- `arithmetic`
- `algebra`
- `logic`
- `predicate`
- `geometry`
- `trigonometry`
- `sequences`

Aktivne domene imaju javna C++ zaglavlja, implementacije, CMake registraciju, testove i prema potrebi WASM adaptere.

### Planirane domene

- `probability_statistics`
- `linear_algebra`
- `mathematical_analysis`
- `discrete_math`
- `complex_numbers`

Planirane domene trenutačno imaju samo README ugovore u `core/include/aksiomat/` i `core/src/`. Ne ulaze u build dok ne dobiju stvarnu implementaciju i testove.

## Konvencije dokumentacije

- Putanje su relativne u odnosu na korijen repozitorija.
- C++ simboli i JavaScript funkcije zapisani su izvornim imenima.
- Primjeri JSON-a predstavljaju ugovor između WASM sloja i frontenda.
- `web/aksiomat.js` i `web/aksiomat.wasm` generirani su build artefakti; ne uređuju se ručno.
- Broj testova naveden u dokumentaciji predstavlja posljednje validirano stanje i može rasti.
