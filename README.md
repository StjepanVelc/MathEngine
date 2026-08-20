# MathEngine

MathEngine je edukacijska matematička web-aplikacija čija se jezgra izvodi u C++20, prevodi u WebAssembly i koristi iz statičkog web sučelja.

## Dokumentacija

Potpuna tehnička dokumentacija nalazi se u [`docs/`](docs/README.md):

- [arhitektura i tok podataka](docs/architecture.md)
- [C++ matematičke domene](docs/core-domains.md)
- [WebAssembly API](docs/wasm-api.md)
- [web frontend](docs/web-frontend.md)
- [zadaci i lokalna pohrana](docs/exercises-and-storage.md)
- [build i pokretanje](docs/build-and-run.md)
- [testiranje i CI](docs/testing-and-ci.md)
- [vodič za proširivanje](docs/extending-mathengine.md)

Ovaj README ostaje sažeti pregled projekta; `/docs` je izvor detaljne razvojne dokumentacije.

## Mogućnosti

- iskazna logika: parsiranje, evaluacija, tablice istinitosti i ekvivalencija
- predikatna logika: predikati, `∀`/`∃`, slobodne varijable i konačne interpretacije
- formalizacija hrvatskih rečenica kroz zadatke po težinama
- modularna aritmetika: napredni kalkulator, točni razlomci, postotci, teorija brojeva i brojevni sustavi
- modularna algebra: simbolički izrazi, jednadžbe, nejednadžbe, sustavi, polinomi i interaktivni grafovi
- geometrija: mjerne jedinice, ravninski likovi, trokuti, Pitagorin poučak, tijela i koordinate
- interaktivni SVG crteži i osnovnoškolska vježbaonica s 22 zadatka
- lokalno spremanje napretka formalizacijskih i geometrijskih zadataka u `localStorage`

## Arhitektura

- `core/include/aksiomat/arithmetic` — aritmetički javni C++ ugovori
- `core/include/aksiomat/algebra` — algebarski javni C++ ugovori
- `core/include/aksiomat/logic` — ugovori iskazne logike
- `core/include/aksiomat/predicate` — ugovori predikatne logike
- `core/include/aksiomat/geometry` — javni ugovori geometrije
- `core/src/{arithmetic,algebra,logic,predicate,geometry}` — implementacije po domenama
- `core/src/wasm_bindings.cpp` — zajednički Emscripten adapter svih domena
- `app` — mala native CLI demonstracija
- `tests` — GoogleTest testovi
- `web` — HTML/CSS/JavaScript frontend i generirani WASM artefakti

Svaka matematička domena ima vlastitu include i source mapu. Predikatna logika razdvojena je na `PredicateExpression`, `PredicateParser` i `Interpretation`, dok `PredicateLogic.hpp` ostaje umbrella header unutar `predicate/` mape.

Obrazovna razina pripada web prezentacijskom sloju, a ne C++ domeni. Isti parser ili solver zato se ne duplicira za osnovnu, srednju i naprednu razinu; frontend samo bira dostupne alate, primjere i količinu objašnjenja.

## Razine učenja

Početni ekran nudi tri razine. Odabir se sprema u `localStorage` i može se promijeniti bez ponovnog učitavanja aplikacije.

### Osnovna škola

- Aritmetika: kalkulator, razlomci, postotci, osnove teorije brojeva i brojevni sustavi
- Osnove algebre: sređivanje izraza, linearne jednadžbe i jednostavne nejednadžbe
- Geometrija: mjere, opseg i površina, trokuti i Pitagora, tijela, koordinate i vježbaonica

### Srednja škola

- Algebra: izrazi, jednadžbe, nejednadžbe, sustavi 2×2, polinomi, funkcije i grafovi
- Logika: iskazna logika i formalizacija rečenica

### Napredno i fakultet

- Matematička logika: iskazna i predikatna logika, normalne forme i interpretacije
- Formalizacija rečenica
- buduće domene: linearna algebra, matematička analiza, diskretna matematika i kompleksni brojevi

Jedan alat može pripadati više razina preko HTML atributa `data-levels`, ali njegova C++ implementacija uvijek ostaje jedinstvena.

## Planirane backend domene

Za budući razvoj pripremljene su dokumentirane mape u `core/include/aksiomat/` i `core/src/`:

- `trigonometry` — kutovi, funkcije, identiteti i trokuti
- `sequences` — aritmetički/geometrijski nizovi, sume i rekurzije
- `probability_statistics` — vjerojatnost, kombinatorika i statistika
- `linear_algebra` — vektori, matrice, determinante i vektorski prostori
- `mathematical_analysis` — limesi, derivacije, integrali i redovi
- `discrete_math` — skupovi, relacije, grafovi i kombinatorika
- `complex_numbers` — algebarski, trigonometrijski i eksponencijalni oblik

Te mape zasad sadrže samo README ugovore. Ne ulaze u CMake dok ne dobiju stvarne C++20 implementacije i GoogleTest testove. Geometrija je već aktivna domena i zato nije na ovom popisu.

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

## Geometrija

Geometrijska jezgra podijeljena je na module `UnitConversion`, `PlaneShapes`, `Triangles`, `Solids` i `Coordinates`. Podržava:

- pretvorbe duljine, površine, obujma i kapaciteta
- opseg i površinu kvadrata, pravokutnika, trokuta, paralelograma, trapeza i kruga
- klasifikaciju trokuta, treći kut i Pitagorin poučak
- oplošje i obujam kocke, kvadra, prizme i valjka
- udaljenost i polovište dviju točaka

Web poglavlje koristi SVG crteže koji se mijenjaju s korisničkim unosom. Vježbaonica učitava 22 zadatka iz `web/data/geometry-exercises.json`, raspoređena na težine `lagano`, `srednje` i `izazov`, te u `localStorage` sprema bodove, točnost, niz i broj rješavanja pojedinih zadataka.

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

## Algebra

Algebra je zaseban podsustav u `core/include/aksiomat/algebra` i `core/src/algebra`. Ne proširuje numerički parser iz `arithmetic/`, nego koristi vlastite module `AlgebraExpression`, `AlgebraParser`, `AlgebraSimplifier`, solvere, `Polynomial` i `FunctionAnalyzer`.

Podržano je:

- parsiranje brojeva, identifikatora, `+`, `-`, `*`, `/`, `^`, zagrada i implicitnog množenja (`2x`, `3(x+1)`)
- pojednostavljivanje konstanti, predznaka, neutralnih elemenata i kompatibilnih članova
- linearne jednadžbe s jednom varijablom i prikazom postupka
- linearne nejednadžbe `<`, `<=`, `>` i `>=` s intervalnim zapisom
- sustavi dviju linearnih jednadžbi s varijablama `x` i `y`
- polinomske operacije, evaluacija, derivacija i realne nultočke do drugog stupnja
- analiza linearnih, kvadratnih i polinomnih funkcija

Web graf koristi Canvas i omogućuje pomicanje, zumiranje, prikaz koordinata, označavanje nultočaka, sjecišta s y-osi i vrha parabole. Parametri `a`, `b` i `c` mogu se mijenjati klizačima, uz glatku animaciju transformacije funkcije i podršku za `prefers-reduced-motion`.

## Testovi i CI

GoogleTest pokriva AST, parsere, tablice istinitosti, ekvivalenciju, arnost predikata, zasjenjivanje varijabli, interpretaciju, aritmetiku, algebru i geometriju. GitHub Actions workflow `.github/workflows/ci.yml` pokreće native testove i zaseban Emscripten build.

## Trenutna ograničenja

- tablica istinitosti ograničena je na šest varijabli
- predikatna logika nema funkcijske simbole
- algebra rješava linearne jednadžbe/nejednadžbe i sustave 2×2; automatske realne nultočke ograničene su na polinome do drugog stupnja
- napredak se čuva samo u trenutnom pregledniku
- SQLite/WASM još nije potreban; uvodi se tek kada model podataka preraste `localStorage`
