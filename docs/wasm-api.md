# WebAssembly API

## Uloga adaptera

`core/src/wasm_bindings.cpp` prevodi javne C++ operacije u funkcije koje JavaScript može pozvati preko Emscripten Embinda. Datoteka se kompajlira samo kada je definiran `__EMSCRIPTEN__`.

Nakon učitavanja:

```javascript
const module = await createAksiomatModule();
const result = module.evaluateArithmetic("2 + 3 * 4");
```

`module` sadrži funkcije registrirane u bloku `EMSCRIPTEN_BINDINGS(aksiomat_module)`.

## Povratni model

API koristi tri oblika povrata:

1. jednostavan tekst, primjerice `"14"`, `"T"` ili normalizirana formula
2. JSON tekst za strukturirani rezultat
3. tekst s prefiksom `GRESKA: ` za neuspjeh

Frontend mora provjeriti prefiks prije `JSON.parse`:

```javascript
const text = module.geometryPlaneShape("rectangle", 7, 3, 0, 0, 0);
if (text.startsWith("GRESKA:")) {
	// prikaži grešku
} else {
	const result = JSON.parse(text);
}
```

`formatDouble` uklanja završne nule iz decimalnog zapisa, a `jsonString` escapira tekst prije umetanja u JSON.

## Iskazna logika

### `logicToString(formula)`

Parsira formulu i vraća normalizirani zapis.

### `logicVariables(formula)`

Vraća varijable formule kao tekst koji frontend koristi za izgradnju kontrola valuacije.

### `logicEvaluate(formula, valuation)`

`valuation` je tekstualni popis dodjela poput `p=1,q=0`. Povrat je `T`, `F` ili greška.

### `truthTable(formula)`

Vraća:

```json
{
  "variables": ["p", "q"],
  "rows": [
	{ "values": [0, 0], "result": 1 }
  ]
}
```

Broj redaka ovisi o broju varijabli i ograničenju jezgre.

### `logicEquivalent(lhs, rhs)`

Vraća `T` ili `F` ovisno o logičkoj ekvivalenciji.

### `logicClassification(formula)`

Vraća klasifikaciju formule i pripadajuće podatke koje `app.js` prikazuje u analitičkoj kartici.

### `logicNormalForms(formula)`

Vraća JSON s NNF, transformiranim CNF/DNF i kanonskim CNF/DNF zapisima.

## Predikatna logika

### `predicateToString(formula)`

Parsira i normalizira predikatnu formulu.

### `predicatePredicates(formula)`

Vraća strukturirani opis:

- predikata i njihovih arnosti
- slobodnih varijabli
- vezanih varijabli

Frontend na temelju arnosti generira moguće n-torke nad zadanom domenom.

### `predicateEvaluate(formula, domain, facts)`

- `domain`: elementi odvojeni zarezom, primjerice `1,2,3`
- `facts`: istinite činjenice odvojene točka-zarezom, primjerice `P(1);Q(1,2)`

Povrat je `T`, `F` ili greška. Adapter pretvara tekstualni model u `Interpretation`, dok semantiku kvantifikatora i predikata provodi C++ jezgra.

## Aritmetika

### `evaluateArithmetic(expression)`

Vraća formatirani brojčani rezultat.

### `rationalCalculate(lhsText, operation, rhsText)`

`operation` je `+`, `-`, `*` ili `/`.

```json
{
  "exact": "5/6",
  "mixed": "5/6",
  "decimal": "0.833333"
}
```

### `rationalFromDecimal(value)`

Pretvara decimalni tekst u točan skraćeni razlomak.

### `percentageCalculate(operation, first, second)`

Podržane vrijednosti `operation`:

| Vrijednost | Značenje |
|---|---|
| `of` | `first` posto od `second` |
| `increase` | povećaj `first` za `second` posto |
| `decrease` | smanji `first` za `second` posto |
| `ratio` | `first` je koliko posto od `second` |
| `originalIncrease` | početna vrijednost prije povećanja |
| `originalDecrease` | početna vrijednost prije smanjenja |

Povrat je formatirani broj kao tekst.

### `numberTheoryAnalyze(valueText)`

```json
{
  "prime": false,
  "divisors": [1, 2, 3, 4, 6, 12],
  "factors": [
	{ "prime": 2, "exponent": 2 },
	{ "prime": 3, "exponent": 1 }
  ]
}
```

### `numberTheoryGcdLcm(lhsText, rhsText)`

```json
{ "gcd": "6", "lcm": "36" }
```

Vrijednosti su tekstualne kako JavaScript ne bi gubio preciznost 64-bitnih cijelih brojeva.

### `convertNumeralSystem(value, fromBase, toBase)`

Vraća broj zapisan u ciljnoj bazi.

## Geometrija

### `geometryConvert(category, value, from, to)`

`category` je `length`, `area` ili `volume`. `from` i `to` su indeksi odgovarajućeg C++ enum tipa.

```json
{ "value": 250 }
```

Mapiranje indeksa:

- duljina: mm=0, cm=1, dm=2, m=3, km=4
- površina: mm²=0, cm²=1, dm²=2, m²=3, ha=4, km²=5
- obujam: mm³=0, cm³=1, dm³=2, m³=3, ml=4, l=5

### `geometryPlaneShape(shape, a, b, c, d, height)`

Podržani `shape` identifikatori:

- `square`
- `rectangle`
- `triangle`
- `parallelogram`
- `trapezoid`
- `circle`

Neiskorišteni argumenti šalju se kao nula; adapter prosljeđuje samo mjere potrebne odabranom liku.

```json
{ "perimeter": 20, "area": 21 }
```

### `geometryTriangle(operation, a, b, c)`

Operacije:

- `classify`
- `thirdAngle`
- `hypotenuse`
- `leg`

Klasifikacija vraća:

```json
{ "bySides": "raznostranican", "byAngles": "pravokutan" }
```

Brojčane operacije vraćaju:

```json
{ "value": 5 }
```

### `geometrySolid(solid, a, b, c)`

Podržani identifikatori su `cube`, `cuboid`, `prism` i `cylinder`.

```json
{ "surfaceArea": 54, "volume": 27 }
```

Kod prizme argumenti predstavljaju površinu baze, opseg baze i visinu.

### `geometryCoordinates(operation, x1, y1, x2, y2)`

`distance` vraća:

```json
{ "distance": 5 }
```

`midpoint` vraća:

```json
{ "x": 2, "y": 1 }
```

## Algebra

### `algebraSimplify(expression)`

```json
{
  "result": "4x + 3",
  "steps": ["..."]
}
```

### `algebraSolveEquation(equation)`

```json
{
  "type": "unique",
  "value": 4,
  "steps": ["..."]
}
```

`type` može biti `unique`, `infinite` ili `none`.

### `algebraSolveInequality(inequality)`

```json
{
  "type": "interval",
  "boundary": 2,
  "relation": "<=",
  "interval": "(-∞, 2]",
  "steps": ["..."]
}
```

### `algebraSolveSystem(first, second)`

Vraća vrstu rješenja, `x`, `y`, zajedničke korake i tri metodska niza:

```json
{
  "type": "unique",
  "x": 2,
  "y": 3,
  "steps": ["..."],
  "methods": {
	"cramer": ["..."],
	"substitution": ["..."],
	"elimination": ["..."]
  }
}
```

### `algebraAnalyzePolynomial(expression)`

Vraća normalizirani polinom, stupanj, derivaciju, nultočke, diskriminantu, vrh i faktorizirani oblik. Neprimjenjive vrijednosti vraćaju se kao JSON `null`.

### `algebraAnalyzeFunction(expression, minX, maxX, sampleCount)`

Vraća analizu funkcije i uzorkovane točke za Canvas graf. `sampleCount` i interval validira C++ `FunctionAnalyzer`.

## Greške i sigurnost ugovora

Svaki adapter slijedi obrazac:

```cpp
try {
	// poziv jezgre i serializacija
} catch (const std::exception& e) {
	return std::string("GRESKA: ") + e.what();
}
```

Posljedice:

- C++ iznimka ne prelazi nekontrolirano u JavaScript.
- Svi frontend moduli mogu koristiti isti način detekcije greške.
- JSON se vraća samo kada je operacija uspjela.
- Matematička validacija ostaje u jezgri.

## Build artefakti

Emscripten cilj koristi:

- `-sMODULARIZE=1`
- `-sEXPORT_NAME=createAksiomatModule`
- `--bind`
- uključene C++ iznimke

Generira:

- `out/build/wasm-release/core/aksiomat.js`
- `out/build/wasm-release/core/aksiomat.wasm`

Post-build naredba kopira obje datoteke u `web/`.

## Pravilo za novi export

Novi export treba:

1. uključiti zaglavlje aktivne C++ domene
2. dodati funkciju u anonimni namespace
3. pozvati samo javni C++ API
4. serializirati rezultat bez ručne matematičke logike
5. uhvatiti `std::exception`
6. registrirati funkciju u `EMSCRIPTEN_BINDINGS`
7. dodati WASM smoke test
8. dokumentirati argumente i povrat u ovom dokumentu
