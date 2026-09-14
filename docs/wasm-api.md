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

## Nizovi i redovi

## Analitička geometrija

### `analyticGeometryVectors(ax, ay, bx, by)`

Vraća duljine, skalarni produkt, determinantu, kut i zbroj dvaju vektora.

### `analyticGeometryLines(x1, y1, x2, y2, x3, y3, x4, y4, px, py)`

Konstruira dva pravca iz parova točaka i vraća njihove koeficijente, odnos, kut, mogući presjek te udaljenost testne točke od prvog pravca.

### `analyticGeometryCircle(mode, first, second, third, fourth, fifth, sixth, px, py)`

`mode` je `centerRadius`, `general` ili `threePoints`. Rezultat uključuje središte, polumjer, opseg, površinu, položaj testne točke i uzorke kružnice.

### `analyticGeometryConic(type, centerX, centerY, first, second, horizontal)`

`type` je `parabola`, `ellipse` ili `hyperbola`. Vraća karakteristične elemente i jednu ili dvije grane uzoraka za SVG prikaz.

## Eksponencijalne i logaritamske funkcije

### `exponentialLogPowersRoots(mode, base, exponent, degree)`

`mode` je `power` ili `root`. Vraća izračunatu vrijednost potencije ili n-tog korijena.

### `exponentialLogFunction(initialValue, base, xMin, xMax, sampleCount)`

Vraća `isGrowth`, `growthRatePercent` i uzorke `{x, y}` funkcije `f(x) = initialValue·baseˣ` na zadanom intervalu.

### `exponentialLogLogarithm(base, xMin, xMax, sampleCount)`

Vraća uzorke `{x, y}` funkcije `f(x) = log_base(x)`. `xMin` mora biti pozitivan.

### `exponentialLogEquation(kind, base, coefficient, target)`

`kind` je `exponential` ili `logarithmic`. Rješava jednostavnu jednadžbu i vraća `solution`.

### `exponentialLogApplication(kind, first, second, third)`

`kind` je `radioactiveDecay`, `phLevel`, `richterMagnitude` ili `soundIntensityDecibels`. Vraća izračunatu vrijednost primjene.

### `sequenceAnalyze(terms)`

Prima članove odvojene zarezom i vraća `terms`, `differences`, `partialSums`, `monotonicity`, `minimum` i `maximum`.

### `sequenceArithmetic(firstTerm, difference, termCount, searchedValue)`

### `sequenceGeometric(firstTerm, ratio, termCount, searchedValue)`

Oba adaptera vraćaju generirane članove, parcijalne sume, zadnji član, konačnu sumu i pronađeni indeks ili `null`. Geometrijski rezultat dodatno sadrži `converges` i `infiniteSum`.

### `sequenceRecurrence(type, first, second, termCount)`

`type` je `arithmetic`, `geometric` ili `fibonacci`. Za prva dva rezultat potvrđuje podudaranje rekurzivnog i eksplicitnog zapisa.

### `sequenceApplication(type, principal, rate, periods, frequency)`

Podržava `simpleInterest`, `compoundInterest`, `population`, `percentage` i `amortization`. Rezultat vraća razvoj vrijednosti kroz razdoblja; amortizacija dodatno vraća ratu, ukupno plaćeno i ukupnu kamatu.

## Kombinatorika, vjerojatnost i statistika

### `combinatoricsCounting(mode, n, k)`

`mode` je `factorial`, `permutations`, `permutationsWithRepetition`, `combinations` ili `combinationsWithRepetition`. Vraća `{ "value": ... }`.

### `combinatoricsProbability(mode, first, second, third)`

`mode` je `classical`, `complement`, `union`, `conditional` ili `independent`. Parametri se tumače prema odabranom načinu (npr. za `classical` su to broj povoljnih i ukupnih ishoda). Vraća `{ "value": ... }`.

### `combinatoricsStatistics(dataset)`

`dataset` je popis brojeva odvojenih zarezom. Vraća srednju vrijednost, medijan, mod (niz), varijancu, standardnu devijaciju i raspon; kada skup ima barem dvije vrijednosti, dodaje i kvartile (`q1`, `q2`, `q3`, `interquartileRange`).

```json
{
  "mean": 4.5,
  "median": 4.5,
  "mode": [1, 2, 3, 4, 5, 6, 7, 8],
  "variance": 5.25,
  "standardDeviation": 2.29,
  "range": 7,
  "q1": 2.5,
  "q2": 4.5,
  "q3": 6.5,
  "interquartileRange": 4
}
```

### `combinatoricsVisualization(dataset, binCount)`

Gradi histogramsku tablicu frekvencija za zadani broj razreda i vraća `{ "bins": [{ "lowerBound", "upperBound", "count" }, ...] }`.

## Matematička analiza (srednjoškolske osnove)

### `calculusLimit(expression, point, variable)`

Vraća `expression`, `point`, `limitValue`, `existsFinite`, `leftSamples`/`rightSamples` (niz `{ "x", "value" }`) i `steps`.

### `calculusDerivative(expression, point, variable)`

Vraća `original`, `derivative`, `pointValue`, `slopeAtPoint`, `tangentLine` i `steps`.

### `calculusRateOfChange(expression, a, b, variable)`

Vraća `averageRate`, `instantaneousRate` i `steps`.

### `calculusDerivativeApplications(expression, variable)`

Vraća `expression`, `derivative`, `criticalPoints` (niz `{ "x", "y", "kind" }` gdje je `kind` `minimum`, `maksimum` ili `prijevojna tocka`), `increasingIntervals`, `decreasingIntervals` i `steps`.

### `calculusDefiniteIntegral(expression, lowerBound, upperBound, variable)`

Vraća `expression`, `antiderivative`, `lowerBound`, `upperBound`, `area`, `numericCheck` (Simpsonova provjera) i `steps`.

Sve `calculus*` funkcije rade isključivo s polinomskim izrazima jedne varijable.

## Matematička analiza (napredno i fakultet)

Funkcije s prefiksom `mathematicalAnalysis*` pokrivaju šest naprednih područja. Za dijelove koji zahtijevaju funkciju kao ulaz (funkcije više varijabli, redovi, diferencijalne jednadžbe, nepravi integral), adapter ne prima proizvoljan izraz nego naziv iz unaprijed definiranog kataloga jer u jezgri ne postoji opći parser izraza s više varijabli.

### `mathematicalAnalysisFormalLimit(expression, point, variable)`

Vraća `expression`, `point`, `limitValue`, `existsFinite`, `epsilonDeltaTable` (niz `{ "epsilon", "delta" }`) i `steps`. Radi isključivo s polinomskim izrazima jedne varijable.

### `mathematicalAnalysisContinuity(expression, point, variable)`

Vraća `isContinuous`, `functionValueAtPoint`, `limitValue` i `steps`.

### `mathematicalAnalysisNthDerivative(expression, order, point, variable)`

`order` je pozitivan cijeli broj. Vraća `derivativeExpression`, `derivativeValueAtPoint` i `steps`.

### `mathematicalAnalysisChainRule(outerExpression, innerExpression, point, variable)`

Vraća `innerValueAtPoint`, `outerDerivativeAtInnerValue`, `innerDerivativeAtPoint`, `compositeDerivativeAtPoint` i `steps` za `(f∘g)'(x) = f'(g(x))·g'(x)`.

### `mathematicalAnalysisImproperIntegral(functionName, lowerBound)`

`functionName` je jedan od kataloških naziva (`gaussian`, `reciprocal_square`, `exp`, `sin`). Vraća `convergent`, `approximateValue` i `steps`.

### `mathematicalAnalysisSubstitutionIntegral(a, b, power, lowerBound, upperBound)`

Računa određeni integral oblika `(ax+b)^n` metodom supstitucije. Vraća `value` i `steps`.

### `mathematicalAnalysisTaylorSeries(functionName, center, order, point)`

`functionName` je `exp` ili `sin`. Vraća `coefficients`, `taylorApproximationAtPoint`, `actualValueAtPoint`, `approximationError` i `steps`.

### `mathematicalAnalysisPowerSeries(coefficientsText, testPoint)`

`coefficientsText` je popis koeficijenata odvojenih zarezom. Vraća `radiusOfConvergence`, `convergesAtPoint` i `steps`.

### `mathematicalAnalysisPartialDerivatives(functionName, x, y)`

`functionName` je jedan od kataloških naziva dvovarijabilnih funkcija (npr. `sum_squares`, `x2y`, `product`). Vraća `partialX`, `partialY`, `gradient` (niz od dva broja) i `gradientMagnitude`.

### `mathematicalAnalysisDirectionalDerivative(functionName, x, y, directionX, directionY)`

Normalizira zadani vektor smjera i vraća `directionalDerivative`, `partialX`, `partialY` i `steps`.

### `mathematicalAnalysisSolveOde(functionName, method, initialT, initialY, finalT, stepSize)`

`method` je `euler` ili `rk4`. `functionName` je naziv iz kataloga desnih strana `y' = f(t,y)` (npr. `exponential_growth`, `logistic`, `harmonic_velocity`). Vraća `points` (niz `{ "t", "y" }`), `finalValue` i `steps`.

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

## Linearna algebra

### `linearAlgebraSpaceVectors(ax, ay, az, bx, by, bz)`

Vraća magnitudu prvog vektora, skalarni i vektorski produkt te njihovu magnitudu za dva vektora u 3D prostoru.

### `linearAlgebraPlaneFromPoints(x1, y1, z1, x2, y2, z2, x3, y3, z3)`

Konstruira ravninu iz tri nekolinearne točke i vraća koeficijente normale i konstantu.

### `linearAlgebraLineVsPlane(x0, y0, z0, dx, dy, dz, normalX, normalY, normalZ, constant)`

Određuje odnos pravca (točka + smjer) i ravnine: leži li pravac na ravnini, je li paralelan te presječnu točku ako postoji.

### `linearAlgebraClassifyQuadricParaboloid(a, b, c)` i `linearAlgebraClassifyQuadricCentral(a, b, c, d)`

Klasificiraju kvadriku (paraboloid odnosno centralnu kvadriku poput elipsoida ili hiperboloida) iz koeficijenata.

### `linearAlgebraMatrixDeterminant(matrixText)`

Računa determinantu kvadratne matrice zapisane kao redci odvojeni s `;` i elementi zarezom.

### `linearAlgebraMatrixInverse(matrixText)`

Vraća invertibilnost, determinantu i inverznu matricu (ako postoji).

### `linearAlgebraMatrixMultiply(leftText, rightText)`

Množi dvije matrice kompatibilnih dimenzija.

### `linearAlgebraMatrixTransform(matrixText, vectorText)`

Primjenjuje matričnu transformaciju na vektor.

### `linearAlgebraEigenSymmetric2x2(a11, a12, a22)` i `linearAlgebraEigenSymmetric3x3(matrixText)`

Računaju svojstvene vrijednosti i pripadne svojstvene vektore simetrične matrice.

### `linearAlgebraLinearIndependence(vectorsText)` i `linearAlgebraExtractBasis(vectorsText)`

Provjeravaju linearnu nezavisnost skupa vektora (uz rang) odnosno izdvajaju bazu i dimenziju razapetog prostora.

## Diskretna matematika

### `discreteMathSetOperation(mode, setAText, setBText)`

`mode` je `union`, `intersection`, `difference` ili `symmetricDifference`. Skupovi su zapisani kao cjelobrojne vrijednosti odvojene zarezom (npr. `"1,2,3"`).

### `discreteMathRelationProperties(domainText, pairsText)`

`domainText` je popis elemenata domene, `pairsText` su uređeni parovi oblika `"1-2;2-3"`. Vraća refleksivnost, simetričnost, antisimetričnost, tranzitivnost te je li relacija ekvivalencija ili parcijalni uređaj.

### `discreteMathGraphAnalyze(adjacencyText, directed)`

Graf je zapisan kao popis susjedstva po vrhu odvojen s `;` (npr. `"1,2;0,2;0,1"`, vrhovi indeksirani od 0). Vraća stupnjeve vrhova, povezanost, Eulerovost i bipartitnost.

### `discreteMathShortestPath(adjacencyText, source, target)`

BFS pretragom vraća dostupnost, udaljenost i put između dva vrha.

### `discreteMathSolveRecurrence(p, q, a0, a1)` i `discreteMathRecurrenceTerms(p, q, a0, a1, count)`

Rješavaju linearnu rekurenciju drugog reda `a(n) = p*a(n-1) + q*a(n-2)` zatvorenom formulom odnosno generiraju prvih `count` članova.

### `discreteMathInclusionExclusionTwo(a, b, ab)` i `discreteMathInclusionExclusionThree(...)`

Računaju veličinu unije skupova principom uključivanja-isključivanja za dva odnosno tri skupa.

### `discreteMathPigeonhole(items, holes)`

Vraća minimalni broj predmeta po pretincu prema Dirichletovom principu.

### `discreteMathDerangements(n)`

Vraća broj derangemana (permutacija bez fiksnih točaka) skupa od `n` elemenata.

## Vjerojatnost i statistika (fakultet)

### `probabilityStatisticsBinomial(trials, probability, successes)`

Vraća vjerojatnost točnog broja uspjeha, očekivanje i varijancu binomne distribucije.

### `probabilityStatisticsPoisson(lambda, occurrences)`

Vraća vjerojatnost, očekivanje i varijancu Poissonove distribucije.

### `probabilityStatisticsNormal(mean, standardDeviation, value)`

Vraća gustoću, kumulativnu vjerojatnost i z-vrijednost normalne distribucije u točki `value`.

### `probabilityStatisticsUniform(lowerBound, upperBound, value)`

Vraća gustoću, kumulativnu vjerojatnost, očekivanje i varijancu uniformne distribucije.

### `probabilityStatisticsConfidenceInterval(sampleText, confidenceLevel, populationStdDev)`

`sampleText` je uzorak brojeva odvojenih zarezom. Vraća uzoročku sredinu, pogrešku procjene te donju/gornju granicu intervala pouzdanosti.

### `probabilityStatisticsZTest(sampleText, hypothesizedMean, populationStdDev, significanceLevel)`

Provodi z-test hipoteze o sredini i vraća testnu statistiku, kritičnu vrijednost i odluku o odbacivanju nulte hipoteze.

### `probabilityStatisticsRegression(xText, yText)`

Računa jednostavnu linearnu regresiju nad parovima `(x, y)` i vraća nagib, odsječak, korelaciju i koeficijent determinacije.

## Kompleksni brojevi

### `complexNumbersOperation(mode, aReal, aImag, bReal, bImag)`

`mode` je `add`, `subtract`, `multiply` ili `divide`. Računa osnovnu operaciju nad dva kompleksna broja.

### `complexNumbersConjugate(real, imaginary)`

Vraća konjugat kompleksnog broja.

### `complexNumbersToPolarForm(real, imaginary)` i `complexNumbersToAlgebraicForm(modulus, argumentDegrees)`

Pretvaraju kompleksni broj između algebarskog i trigonometrijskog (polarnog) oblika.

### `complexNumbersPower(real, imaginary, exponent)` i `complexNumbersNthRoots(real, imaginary, degree)`

Računaju cjelobrojnu potenciju (De Moivre) odnosno sve n-te korijene kompleksnog broja.

### `complexNumbersSolveQuadratic(a, b, c)`

Rješava kvadratnu jednadžbu s realnim koeficijentima; kada je diskriminanta negativna, rješenja su kompleksna.

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
