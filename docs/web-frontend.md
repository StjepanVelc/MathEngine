# Web frontend

## Pregled

Frontend je statička HTML/CSS/JavaScript aplikacija u mapi `web/`. Ne koristi framework ni bundler. Skripte se učitavaju redoslijedom iz `web/index.html`, dijele globalni Emscripten modul i svaka inicijalizira samo svoju domenu.

## Učitavanje aplikacije

Relevantni redoslijed skripti:

1. `aksiomat.js` — generirani Emscripten loader
2. `formalization.js`
3. `predicates.js`
4. `arithmetic.js`
5. `geometry-visuals.js`
6. `geometry-practice.js`
7. `geometry.js`
8. `trigonometry-visuals.js`, `trigonometry-practice.js`, `trigonometry.js`
9. `sequences-visuals.js`, `sequences-practice.js`, `sequences.js`
10. `graph.js`
11. `algebra.js`
12. `app.js`

Domenski moduli definiraju globalne setup funkcije prije nego što `app.js` pozove `init()`.

```mermaid
sequenceDiagram
	participant H as index.html
	participant E as aksiomat.js
	participant A as app.js
	participant W as WebAssembly
	participant M as Domenski moduli

	H->>E: učitavanje loadera
	H->>M: registracija setup funkcija
	H->>A: učitavanje app.js
	A->>E: createAksiomatModule()
	E->>W: instanciranje aksiomat.wasm
	W-->>A: module
	A->>M: setup*(module)
```

Ako `createAksiomatModule` nije dostupan, statusni element prikazuje uputu za WASM build i ostatak aplikacije se ne inicijalizira.

## `web/index.html`

HTML definira:

- zaglavlje aplikacije
- status učitavanja
- odabir obrazovne razine
- poglavlja i pod-panele
- obrasce i rezultate
- Canvas element algebarskog grafa
- spremnike za geometrijske SVG crteže
- jedan korijenski spremnik za dinamičku geometrijsku vježbaonicu

### Konvencija poglavlja

Poglavlje koristi:

```html
<section class="chapter" data-levels="primary secondary" hidden>
	<button class="chapter-toggle" aria-expanded="false">...</button>
	<div class="chapter-body" hidden>...</div>
</section>
```

`data-levels` je popis razina odvojenih razmakom. Element bez tog atributa smatra se dostupnim na svim razinama.

### Konvencija panela

Svaka domena ima vlastitu klasu gumba i panela:

- `.subchapter-toggle` / `.subchapter-panel`
- `.arithmetic-toggle` / `.arithmetic-panel`
- `.algebra-toggle` / `.algebra-panel`
- `.geometry-toggle` / `.geometry-panel`
- `.trigonometry-toggle` / `.trigonometry-panel`
- `.sequences-toggle` / `.sequences-panel`

Gumb preko `data-panel` pokazuje na `id` pripadajućeg panela.

## `web/app.js`

`app.js` je zajednički orkestrator.

### Obrazovne razine

`learningLevels` mapira interne identifikatore:

- `primary` → Osnovna škola
- `secondary` → Srednja škola
- `advanced` → Napredno i fakultet

`supportsLevel(element, level)` provjerava `data-levels`.

`setupLearningLevels()`:

- čita spremljenu razinu iz `localStorage`
- prikazuje odabir ako razina nije postavljena
- skriva nepodržana poglavlja, gumbe i panele
- zatvara ranije otvoren sadržaj
- sprema novi izbor
- ažurira oznaku aktivne razine i gumb za promjenu škole

Ključ pohrane je `mathengine-learning-level`.

Važno: filtriranje ne mijenja C++ API niti stvara zasebne matematičke implementacije.

### Poglavlja i podpoglavlja

`setupChapters()` kontrolira otvaranje tijela poglavlja i ažurira `aria-expanded` te znak `▸/▾`.

`setupSubchapters()` osigurava da je unutar logike otvoren najviše jedan panel.

Aritmetika, algebra i geometrija imaju istu logiku u vlastitim setup funkcijama jer upravljaju domenski specifičnim klasama.

## Frontend nizova i redova

`sequences.js` upravlja sa šest srednjoškolskih panela, formatira WASM JSON, gradi tablice članova i obrazovne korake. `sequences-visuals.js` crta diskretne SVG točke, parcijalne sume, granicu konvergentnog reda, rekurzivne veze i razvoj vrijednosti kroz vrijeme. `sequences-practice.js` dinamički stvara vježbaonicu i učitava 24 zadatka iz `web/data/sequences-exercises.json`.

### Palete simbola

`setupPalette(palette, onInsert)` umeće simbol na trenutačnu poziciju kursora u ciljani input. Cilj se određuje atributom `data-target`. Nakon umetanja opcionalno poziva callback za ponovnu evaluaciju.

### Bootstrap

`init()`:

1. provjerava Emscripten loader
2. čeka `createAksiomatModule()`
3. skriva status
4. inicijalizira razine i navigaciju
5. predaje isti `module` svim domenskim setup funkcijama

## `web/arithmetic.js`

`setupArithmetic(module)` upravlja s pet panela.

### Zajednički helperi

- `requireValues` provjerava obavezna polja
- `renderRows` gradi standardne kartice rezultata
- `renderError` prikazuje grešku bez pokušaja parsiranja JSON-a

### Paneli

- Kalkulator: poziva `evaluateArithmetic` pri kliku ili Enteru.
- Razlomci: poziva `rationalCalculate` i `rationalFromDecimal`.
- Postotci: operacija iz selecta izravno odgovara WASM identifikatoru.
- Teorija brojeva: prikazuje prostost, djelitelje, faktorizaciju, NZD i NZV.
- Brojevni sustavi: šalje tekst broja i dvije baze.

## `web/algebra.js`

`setupAlgebra(module)` upravlja izrazima, jednadžbama, nejednadžbama, sustavima, polinomima i funkcijama.

Odgovornosti:

- pretvaranje API JSON-a u edukativne kartice
- školski zapis operatora i predznaka
- prikaz koraka
- prikaz tri metode sustava
- crtanje brojevne crte za nejednadžbe
- predaja analize funkcije modulu `graph.js`

Algebarski frontend ne rješava jednadžbu samostalno.

## `web/graph.js`

`setupAlgebraGraph(module)` upravlja Canvas grafom.

Funkcionalnosti:

- responzivna veličina platna
- mreža i osi
- pan povlačenjem
- zoom kotačićem
- prikaz koordinata kursora
- animirani prijelaz između funkcija
- karakteristične točke: nultočke, sjecište s y-osi i vrh
- klizači parametara `a`, `b`, `c`
- `prefers-reduced-motion` podrška

Matematičke točke i uzorke dobiva iz `algebraAnalyzeFunction`.

## `web/predicates.js`

`setupPredicates(module)`:

1. parsira formulu i čita opis predikata
2. prima konačnu domenu
3. generira moguće n-torke prema arnosti predikata
4. dopušta korisniku označavanje istinitih činjenica
5. serializira domenu i činjenice za `predicateEvaluate`

Radi zaštite preglednika broj prikazanih n-torki ograničen je na 256.

## `web/formalization.js`

Formalizacijski modul učitava `web/data/formalization-exercises.json`, prikazuje rečenicu i legendu, a korisničku formulu provjerava funkcijom `logicEquivalent`.

Modul vodi statistiku po težini, omogućuje reset trenutačne ili ukupne statistike i sprema stanje lokalno.

## `web/geometry.js`

`setupGeometry(module)` je kontroler svih geometrijskih alata.

### Organizacija

- panel navigacija
- čitanje i validacija brojčanih inputa
- standardizirani `render`, `fail` i `parse` helperi
- lokalizirano formatiranje kroz `Intl.NumberFormat("hr-HR")`
- dinamičko stvaranje inputa prema odabranoj operaciji
- pozivanje odgovarajućeg Geometry WASM adaptera
- osvježavanje SVG prikaza
- inicijalizacija vježbaonice

### Dinamička polja

Definicijske mape `shapeDefinitions`, `triangleDefinitions` i `solidDefinitions` sadrže:

- ključeve polja
- korisničke oznake
- početne vrijednosti
- formule za edukativnu karticu

`createFields` na temelju definicije izgrađuje inpute. Time HTML ne mora sadržavati zaseban obrazac za svaku varijantu lika ili tijela.

### Pretvorbe jedinica

Mapa `units` mora ostati usklađena s redoslijedom C++ enum vrijednosti dokumentiranim u [WebAssembly API-ju](wasm-api.md).

## `web/geometry-visuals.js`

`GeometryVisuals` je IIFE koji vraća javne metode:

- `units`
- `shape`
- `triangle`
- `solid`
- `coordinates`
- `refresh`

Modul koristi SVG DOM API (`createElementNS`) umjesto umetanja proizvoljnog SVG teksta. Helperi grade linije, oznake, poligone i zajedničke boje.

Vizualizacije su pedagoške i skalirane za čitljivost; nisu tehnički crteži u stvarnom mjerilu. Brojčane vrijednosti služe za oznake i relativni oblik gdje je primjenjivo.

CSS klase `.geometry-draw` i `.geometry-pop` daju kratke ulazne animacije, isključene kada korisnik preferira smanjeno kretanje.

## `web/geometry-practice.js`

Vježbaonica se ne nalazi cijela u statičkom HTML-u. `index.html` sadrži samo:

```html
<div id="geometry-practice-root"></div>
```

`setupGeometryPractice` umeće puni template u taj korijen preko `root.innerHTML`. Zato DOM validacija mora pregledati i dinamički template, a ne samo `index.html`.

Nakon stvaranja elemenata modul:

- učitava JSON banku
- veže događaje
- bira zadatak
- provjerava odgovor s tolerancijom
- prikazuje trag i rješenje
- ažurira bodove i statistiku
- sprema stanje

Detaljan model opisan je u [Zadaci i lokalna pohrana](exercises-and-storage.md).

## Trigonometrijski frontend

`trigonometry.js` upravlja srednjoškolskim panelima, dinamičkim obrascima i WASM rezultatima. `trigonometry-visuals.js` povezuje isti kut s jediničnom kružnicom, projekcijama sin/cos, valnim grafom i označenim trokutima. `trigonometry-practice.js` dinamički stvara vježbaonicu i učitava 24 zadatka iz `web/data/trigonometry-exercises.json`.

## `web/style.css`

Jedna datoteka trenutno sadrži zajednički dizajn svih domena.

Glavne skupine stilova:

- osnovna tema i layout
- zaglavlje i obrazovne razine
- poglavlja i navigacija
- obrasci, palete i kartice rezultata
- logičke tablice i predikatni editor
- algebarske metode, brojevna crta i Canvas graf
- geometrijski hero, laboratorijski layout i SVG
- vježbaonica i responzivni prikaz

Geometrija koristi lokalne CSS varijable unutar `.geometry-chapter` kako ne bi mijenjala globalnu temu.

## Pristupačnost

Postojeći frontend koristi:

- nativne `button`, `label`, `input`, `select`, `output` elemente
- `aria-expanded` na poglavljima
- `aria-pressed` na karticama razine
- `aria-live` za povratnu informaciju vježbaonice
- tekstualne oznake za Canvas/SVG spremnike
- `prefers-reduced-motion`

Pri dodavanju novog UI-ja treba zadržati upravljanje tipkovnicom i ne oslanjati se samo na boju za značenje rezultata.

## Pravila frontend razvoja

1. Matematičku formulu ne duplicirati u JavaScriptu ako postoji C++ API.
2. Prije `JSON.parse` uvijek provjeriti `GRESKA:`.
3. Dinamički tekst postavljati kroz `textContent`; `innerHTML` koristiti samo za kontrolirane lokalne templateove.
4. Novi panel povezati s `data-levels` filtriranjem.
5. Novi globalni setup učitati prije `app.js` i pozvati iz `init()`.
6. Generirani `aksiomat.js` ne uređivati ručno.
7. Pokrenuti `scripts/validate_web.mjs` nakon promjene DOM-a, skripti ili JSON banki.
