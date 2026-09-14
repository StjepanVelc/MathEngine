# Zadaci i lokalna pohrana

## Zašto su zadaci izvan C++ jezgre

Tekst zadatka, trag, objašnjenje i razina težine pedagoški su sadržaj. Smješteni su u JSON kako bi se mogli mijenjati bez C++ rekompilacije i WASM builda. C++ jezgra i dalje provodi formalnu matematičku provjeru kada je potrebna.

Trenutačno postoji devet banaka:

- `web/data/formalization-exercises.json`
- `web/data/geometry-exercises.json`
- `web/data/trigonometry-exercises.json`
- `web/data/sequences-exercises.json`
- `web/data/analytic-geometry-exercises.json`
- `web/data/exponential-logarithmic-exercises.json`
- `web/data/combinatorics-probability-statistics-exercises.json`
- `web/data/calculus-basics-exercises.json`
- `web/data/mathematical-analysis-exercises.json`

Trigonometrijska banka ima razine `temelji`, `primjena` i `izazov`, ukupno 24 zadatka. Koristi isti brojčani model odgovora i tolerancije kao geometrija, a stanje sprema pod ključem `mathengine-trigonometry-practice-v1`.

Banka nizova također ima 24 zadatka kroz `temelji`, `primjena` i `izazov`. Pokriva članove, sume, rekurzije, Fibonacci, kamate, populaciju, amortizaciju i ponavljane postotke. Napredak sprema pod `mathengine-sequences-practice-v1`.

Banka Analitičke geometrije sadrži 24 zadatka iz vektora, pravaca, udaljenosti, kružnica i konika. Koristi isti brojčani model odgovora, a napredak sprema pod `mathengine-analytic-geometry-practice-v1`.

Banka eksponencijalnih i logaritamskih funkcija sadrži 24 zadatka iz potencija, korijena, eksponencijalnog rasta/pada, logaritama, jednadžbi i primjena (pH, Richterova skala, radioaktivni raspad, decibeli). Koristi isti brojčani model odgovora, a napredak sprema pod `mathengine-exponential-logarithmic-practice-v1`.

Banka kombinatorike, vjerojatnosti i statistike sadrži 24 zadatka kroz `temelji`, `primjena` i `izazov`, pokrivajući prebrojavanje, klasičnu/uvjetnu/nezavisnu vjerojatnost te deskriptivnu statistiku. Koristi isti brojčani model odgovora, a napredak sprema pod `mathengine-combinatorics-probability-statistics-practice-v1`.

Banka matematičke analize (srednjoškolske osnove) sadrži 24 zadatka kroz `temelji`, `primjena` i `izazov`, pokrivajući limes, derivaciju, primjene derivacije (monotonost/ekstremi) i određeni integral. Koristi isti brojčani model odgovora, a napredak sprema pod `mathengine-calculus-basics-practice-v1`.

Banka matematičke analize (napredno i fakultet) sadrži 17 zadataka kroz `temelji`, `primjena` i `izazov`, pokrivajući formalni limes/kontinuitet, viši red derivacije, lančano pravilo, integraciju supstitucijom, konvergenciju nepravog integrala, Taylorov red, radijus konvergencije reda potencija, parcijalne/usmjerene derivacije te Eulerovu/RK4 metodu. Koristi isti brojčani model odgovora, a napredak sprema pod `mathengine-mathematical-analysis-practice-v1`.

Banke se učitavaju preko `fetch`, pa aplikaciju treba pokretati preko HTTP servera, ne izravno protokolom `file://`.

## Formalizacijski zadaci

### Struktura banke

Banka je podijeljena po težinama. Svaki zadatak sadrži:

```json
{
  "id": "jedinstveni-identifikator",
  "sentence": "Hrvatska rečenica koju treba formalizirati",
  "legend": "p: ..., q: ...",
  "solution": "p → q",
  "explanation": "Objašnjenje zašto formula odgovara rečenici"
}
```

Polja:

| Polje | Namjena |
|---|---|
| `id` | stabilni ključ zadatka i statistike |
| `sentence` | prirodni jezik prikazan korisniku |
| `legend` | značenje iskaznih varijabli |
| `solution` | referentna logička formula |
| `explanation` | pedagoško obrazloženje |

### Provjera odgovora

Odgovor se ne uspoređuje tekstualno. `formalization.js` poziva:

```javascript
module.logicEquivalent(userFormula, exercise.solution)
```

Zato su logički ekvivalentni zapisi prihvaćeni čak i kada nisu identični referentnom tekstu.

Primjer: ako je rješenje `p → q`, odgovor `¬p ∨ q` također je točan.

### Odabir i statistika

Modul održava trenutačnu težinu, nasumično bira zadatak iz odgovarajuće skupine i ažurira broj pokušaja/točnih odgovora. Korisnik može resetirati statistiku trenutačne težine ili cijelu statistiku.

### Pohrana

Ključ:

```text
mathengine.formalization.progress
```

Vrijednost je JSON objekt sa statistikom po težinama. Kod učitavanja se nepostojanje ključa tretira kao prazan objekt.

## Geometrijski zadaci

### Struktura banke

`geometry-exercises.json` ima tri skupine:

- `lagano`
- `srednje`
- `izazov`

Ukupno je validirano 22 zadatka.

Primjer:

```json
{
  "id": "g-s-01",
  "topic": "Pitagora",
  "title": "Ljestve uz zid",
  "question": "Ljestve su udaljene 6 m od zida...",
  "answer": 10,
  "unit": "m",
  "tolerance": 0.001,
  "hint": "Ljestve su hipotenuza pravokutnog trokuta.",
  "explanation": "c = √(6² + 8²) = √100 = 10 m."
}
```

| Polje | Namjena |
|---|---|
| `id` | stabilni jedinstveni identifikator |
| `topic` | kratka oznaka područja |
| `title` | naslov problemske situacije |
| `question` | puni tekst zadatka |
| `answer` | očekivani brojčani odgovor |
| `unit` | jedinica prikazana uz unos i rješenje |
| `tolerance` | dopuštena apsolutna numerička razlika |
| `hint` | trag na zahtjev korisnika |
| `explanation` | postupak nakon provjere |

### Čitanje odgovora

Korisnički unos:

1. uklanja okolne razmake
2. zamjenjuje decimalni zarez točkom
3. pretvara se u `Number`
4. odbija se ako nije konačan broj

Provjera koristi:

```javascript
Math.abs(userAnswer - exercise.answer) <= exercise.tolerance
```

Tolerancija omogućuje zadatke s aproksimacijom π i decimalnim rezultatom.

### Algoritam izbora

`chooseExercise()`:

1. uzima zadatke trenutačne težine
2. isključuje neposredno prethodni zadatak kada postoje druge mogućnosti
3. pronalazi najmanji broj ranijih uspješnih rješavanja
4. bira samo među najmanje riješenim zadacima
5. nasumično odabire iz tog skupa

Time se izbjegava neposredno ponavljanje i postupno pokriva cijela banka.

### Bodovanje

Osnovni bodovi po težini:

- lagano: 10
- srednje: 20
- izazov: 30

Točan odgovor dodatno dobiva bonus za niz, do ograničenog broja uzastopnih odgovora. Netočan odgovor prekida trenutačni niz.

Praćene vrijednosti:

- broj pokušaja
- broj točnih odgovora
- trenutačni niz
- najbolji niz
- bodovi
- broj uspješnih rješavanja svakog zadatka

Točnost u UI-ju računa se kao `correct / attempts`.

### Pohrana

Ključ:

```text
mathengine-geometry-practice-v1
```

Model:

```json
{
  "attempts": 12,
  "correct": 9,
  "streak": 2,
  "best": 5,
  "points": 146,
  "completed": {
	"g-l-01": 2,
	"g-s-01": 1
  }
}
```

Sufiks `v1` označava verziju sheme. Ako se model nekompatibilno promijeni, preporučuje se novi ključ ili eksplicitna migracija.

Kod oštećenog JSON-a `loadState()` vraća početno stanje umjesto prekida aplikacije.

## Odabrana obrazovna razina

`app.js` koristi zaseban ključ:

```text
mathengine-learning-level
```

Vrijednost je jedan od identifikatora `primary`, `secondary` ili `advanced`.

## Pravila za nove zadatke

### Formalizacija

- `id` mora biti jedinstven i stabilan.
- `solution` mora biti formula koju C++ logički parser prihvaća.
- legenda mora jednoznačno opisivati svaku varijablu.
- objašnjenje treba objasniti strukturu, a ne samo ponoviti rješenje.

### Geometrija

- `id` mora biti jedinstven u svim težinama.
- `answer` mora biti broj, ne lokalizirani tekst.
- `unit` se ne upisuje u brojčani odgovor.
- `tolerance` treba biti što manja, ali dovoljna za očekivano zaokruživanje.
- pitanje mora sadržavati sve potrebne podatke.
- trag ne smije odmah otkriti konačan rezultat.
- objašnjenje mora prikazati formulu, uvrštavanje i zaključak.

## Sigurnost i privatnost

`localStorage` ostaje samo u trenutnom pregledniku i originu. Aplikacija ne šalje statistiku poslužitelju. Reset uklanja ili prepisuje lokalno spremljeno stanje.

Ograničenja:

- napredak nije sinkroniziran između uređaja
- brisanje podataka preglednika briše napredak
- promjena origin-a ili porta može stvoriti odvojenu pohranu
- `localStorage` nije baza za osjetljive podatke

## Validacija banki

`scripts/validate_web.mjs` provjerava:

- valjan JSON obje banke
- postojanje tri geometrijske težine
- ukupan broj geometrijskih zadataka
- jedinstvenost geometrijskih ID-jeva

Za semantičku kvalitetu zadataka i točnost objašnjenja i dalje je potreban stručni pregled.
