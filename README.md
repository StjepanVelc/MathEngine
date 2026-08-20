# MathEngine

MathEngine je edukacijska matematička web-aplikacija čija se jezgra izvodi u C++20, prevodi u WebAssembly i koristi iz statičkog web sučelja.

## Mogućnosti

- iskazna logika: parsiranje, evaluacija, tablice istinitosti i ekvivalencija
- predikatna logika: predikati, `∀`/`∃`, slobodne varijable i konačne interpretacije
- formalizacija hrvatskih rečenica kroz zadatke po težinama
- modularna aritmetika: napredni kalkulator, točni razlomci, postotci, teorija brojeva i brojevni sustavi
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

## Aritmetika

Aritmetička jezgra podijeljena je na neovisne module:

- `ArithmeticExpression`, `ArithmeticParser`, `ArithmeticEvaluator` — napredni kalkulator
- `Rational` — točni razlomci
- `Percentages` — postotni izračuni
- `NumberTheory` — prostost, djelitelji, faktorizacija, NZD i NZV
- `NumeralSystems` — pretvorbe između baza 2–36

`Arithmetic` ostaje kompatibilna fasada koja parsira i evaluira izraz.

### Sintaksa kalkulatora

Podržani su operatori `+`, `-`, `*`, `/`, `%`, `^` i postfiksni faktorijel `!`. Potenciranje je desno asocijativno, a standardna matematička konvencija daje `-2^2 = -4`.

Konstante:

- `pi`
- `e`

Funkcije:

- `sqrt(x)`, `abs(x)`
- `min(x,y)`, `max(x,y)`, `mod(x,y)`
- `round(x)`, `floor(x)`, `ceil(x)`

Podržan je znanstveni zapis, primjerice `1.5e6`. Primjer složenijeg izraza: `-2^2 + sqrt(16) * 3!`.

### Razlomci

Razlomci koriste 64-bitne cijele brojeve, automatski se skraćuju i čuvaju točan rezultat. Web prikazuje obični razlomak, mješoviti broj i decimalnu aproksimaciju. Decimalni zapis može se pretvoriti u točan razlomak.

### Postotci

Podržani su postotak vrijednosti, povećanje/smanjenje, omjer u postocima te izračun početne vrijednosti prije postotne promjene.

### Teorija brojeva

Podržani su provjera prostosti, sortirani djelitelji, rastav na proste faktore, najveći zajednički djelitelj i najmanji zajednički višekratnik.

### Brojevni sustavi

Potpisani 64-bitni cijeli brojevi mogu se pretvarati između baza 2–36. Izlaz koristi znamenke `0-9` i velika slova `A-Z`.

## Testovi i CI

GoogleTest pokriva AST, parsere, tablice istinitosti, ekvivalenciju, arnost predikata, zasjenjivanje varijabli, interpretaciju i aritmetiku. GitHub Actions workflow `.github/workflows/ci.yml` pokreće native testove i zaseban Emscripten build.

## Trenutna ograničenja

- tablica istinitosti ograničena je na šest varijabli
- predikatna logika nema funkcijske simbole ni jednakost
- napredak se čuva samo u trenutnom pregledniku
- SQLite/WASM još nije potreban; uvodi se tek kada model podataka preraste `localStorage`
