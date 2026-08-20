# MathEngine

MathEngine je edukacijska matematička web-aplikacija čija se jezgra izvodi u C++20, prevodi u WebAssembly i koristi iz statičkog web sučelja.

## Mogućnosti

- iskazna logika: parsiranje, evaluacija, tablice istinitosti i ekvivalencija
- predikatna logika: predikati, `∀`/`∃`, slobodne varijable i konačne interpretacije
- formalizacija hrvatskih rečenica kroz zadatke po težinama
- aritmetički izrazi s prioritetom operacija, zagradama i decimalnim brojevima
- lokalno spremanje napretka formalizacijskih zadataka u `localStorage`

## Arhitektura

- `core/include/aksiomat` — javni C++ ugovori
- `core/src` — implementacija matematičke jezgre i Emscripten bindings
- `app` — mala native CLI demonstracija
- `tests` — GoogleTest testovi
- `web` — HTML/CSS/JavaScript frontend i generirani WASM artefakti

Predikatna logika razdvojena je na `PredicateExpression`, `PredicateParser` i `Interpretation`. `PredicateLogic.hpp` ostaje kompatibilni umbrella header.

## Zahtjevi

- CMake 3.14 ili noviji
- C++20 compiler
- Ninja
- za Windows native build: Visual Studio/MSVC developer shell
- za WASM build: Emscripten SDK

## Native build na Windowsu

Pokrenuti iz Visual Studio Developer PowerShella:

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
ctest --test-dir out/build/x64-debug --output-on-failure
```

Release konfiguracija koristi preset `x64-release`. Dostupni su i x86 preseti.

## WebAssembly build

Nakon aktiviranja emsdk okruženja:

```powershell
cmake --preset wasm-release
cmake --build out/build/wasm-release
```

Build automatski kopira `aksiomat.js` i `aksiomat.wasm` u `web/`.

## Pokretanje web-aplikacije

Web-aplikaciju treba pokrenuti preko HTTP servera jer WASM i JSON zadaci ne rade pouzdano preko `file://` protokola:

```powershell
cd web
python -m http.server 8000
```

Zatim otvoriti `http://localhost:8000`.

## Sintaksa

### Iskazna logika

| Operacija | Unicode | ASCII |
|---|---:|---:|
| Negacija | `¬` | `!` |
| Konjunkcija | `∧` | `&` |
| Disjunkcija | `∨` | `|` |
| Implikacija | `→` | `->` |
| Ekvivalencija | `↔` | `<->` |

Primjer: `(p ∧ ¬q) → r` ili `(p & !q) -> r`.

### Predikatna logika

Kvantifikatori se pišu kao `∀x`, `∃x`, `forall x` ili `exists x`. Predikati imaju jedan ili više argumenata, primjerice `Paran(x)` i `Manji(x, y)`.

Primjer: `∀x ∃y Manji(x, y)`.

Isti predikat mora u cijeloj formuli imati jednaku arnost. Konačna domena mora biti neprazna i sadržavati jedinstvene elemente. Web sučelje automatski generira moguće n-torke predikata; radi zaštite preglednika prikazuje najviše 256 n-torki.

## Formalizacijski zadaci

Zadaci se nalaze u `web/data/formalization-exercises.json`. Svaki zadatak ima jedinstveni `id`, rečenicu, legendu, rješenje i objašnjenje. Napredak se čuva lokalno po težini.

## Testovi i CI

GoogleTest pokriva AST, parsere, tablice istinitosti, ekvivalenciju, arnost predikata, zasjenjivanje varijabli, interpretaciju i aritmetiku. GitHub Actions workflow `.github/workflows/ci.yml` pokreće native testove i zaseban Emscripten build.

## Trenutna ograničenja

- tablica istinitosti ograničena je na šest varijabli
- predikatna logika nema funkcijske simbole ni jednakost
- napredak se čuva samo u trenutnom pregledniku
- SQLite/WASM još nije potreban; uvodi se tek kada model podataka preraste `localStorage`
