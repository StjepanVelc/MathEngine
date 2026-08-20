# Proširivanje MathEnginea

## Cilj

Ovaj vodič definira standardni postupak za dodavanje nove funkcionalnosti bez narušavanja granica između matematičke jezgre, WASM adaptera i prezentacije.

## Prvo odredi vrstu promjene

### Proširenje postojeće domene

Primjeri:

- nova funkcija u aritmetičkom evaluatoru
- novi geometrijski lik
- nova algebarska analiza

Kod ostaje u postojećim include/source mapama i dobiva ciljane testove.

### Nova aktivna domena

Primjeri:

- trigonometrija
- kompleksni brojevi
- vjerojatnost i statistika

Zahtijeva javni API, implementaciju, CMake registraciju, testove, po potrebi WASM i frontend.

### Samo pedagoški sadržaj

Primjeri:

- novi zadaci
- bolja objašnjenja
- nova razina težine

Ako ne postoji nova matematička operacija, promjena može ostati u JSON-u i frontendu.

## 1. Dizajn javnog C++ API-ja

Kreirati zaglavlja u:

```text
core/include/aksiomat/<nova_domena>/
```

Javni API treba:

- koristiti C++20 i standardne tipove
- biti neovisan o Emscriptenu i webu
- imati jasne ulazne preduvjete
- vratiti strukturirani rezultat kada postoji više vrijednosti
- koristiti enum za konačan skup operacija ili klasifikacija
- koristiti `std::optional` kada rezultat legitimno ne postoji
- baciti standardnu iznimku za neispravan ulaz

Primjer strukture:

```cpp
namespace aksiomat::nova_domena {

struct Result {
	double value;
	std::string classification;
};

class Analyzer {
public:
	static Result analyze(double input);
};

}
```

Ne stavljati HTML, lokalizirane UI oznake ni `emscripten::val` u javni ugovor.

## 2. Implementacija

Izvorne datoteke smjestiti u:

```text
core/src/<nova_domena>/
```

Pravila:

- matematičke formule implementirati samo ovdje
- validaciju držati uz algoritam
- ne prikrivati nemoguć matematički slučaj nulom ili praznim tekstom
- koristiti `std::numbers` za standardne konstante
- koristiti numeričku toleranciju samo kada je opravdana
- održavati funkcije malima i testabilnima

## 3. CMake registracija

Dodati svaki `.cpp` u `add_library(aksiomat_core STATIC ...)` u `core/CMakeLists.txt`.

Placeholder README mapa nije aktivna domena i sama se ne registrira. Kada domena postane aktivna, njezin README treba ažurirati iz planiranog u stvarni status.

Nakon promjene pokrenuti configure/build kako bi CMake regenerirao Ninja graf.

## 4. GoogleTest testovi

Kreirati:

```text
tests/test_<nova_domena>_<područje>.cpp
```

Registrirati datoteku u `tests/CMakeLists.txt`.

Minimalna testna matrica:

| Kategorija | Primjer |
|---|---|
| standardni slučaj | poznati ulaz i rezultat |
| rubna vrijednost | nula, jedinica, granica intervala |
| neispravan ulaz | očekivani `std::invalid_argument` |
| klasifikacija | svaka vrijednost enum rezultata |
| preciznost | `EXPECT_NEAR` za iracionalne rezultate |
| overflow | kada API koristi ograničene cijele brojeve |

Najprije pokrenuti ciljane, zatim sve testove.

## 5. WASM adapter

Ako funkcionalnost treba web, urediti `core/src/wasm_bindings.cpp`.

Postupak:

1. uključiti javno zaglavlje
2. dodati adaptersku funkciju u anonimni namespace
3. prihvatiti JavaScript-friendly argumente
4. mapirati tekstualni identifikator na C++ enum ili metodu
5. pozvati C++ API
6. serializirati složeni rezultat u valjani JSON
7. uhvatiti `std::exception`
8. registrirati kroz `emscripten::function`

Primjer obrasca:

```cpp
std::string domainAnalyze(std::string operation, double value) {
	try {
		if (operation != "analyze") {
			throw std::invalid_argument("Nepoznata operacija");
		}
		const auto result = Domain::analyze(value);
		return "{\"value\":" + formatDouble(result.value) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}
```

Ne kopirati formulu u adapter. Ako adapter sadrži stvarni matematički algoritam, granica je pogrešno postavljena.

Nakon toga:

- napraviti WASM build
- učitati modul iz Nodea
- pozvati uspješan i neuspješan slučaj
- parsirati svaki očekivani JSON
- ažurirati [WebAssembly API](wasm-api.md)

## 6. Web panel

### HTML

Dodati poglavlje ili panel s:

- jedinstvenim ID-jevima
- `hidden` početnim stanjem
- `data-levels`
- semantičkim labelama
- spremnikom rezultata
- pristupačnim nazivom vizualizacije

### JavaScript

Preporučeni oblik:

```javascript
function setupDomain(module) {
	// cache DOM elemenata
	// helperi validacije i prikaza
	// event listeneri
	// poziv module.domainAnalyze(...)
}
```

Skriptu učitati prije `app.js`, a setup funkciju pozvati nakon nastanka WASM modula.

Obavezno:

- razlikovati prazan unos od numeričke nule
- provjeriti `GRESKA:` prije `JSON.parse`
- koristiti `textContent` za podatke korisnika i API-ja
- ne duplicirati C++ validaciju kao jedini izvor istine
- koristiti postojeće kartice i CSS konvencije

### Obrazovne razine

Razinu definirati u prezentaciji:

```html
<section class="chapter" data-levels="secondary advanced">
```

Ako isti alat treba drugačije objašnjenje po razini, mijenjati primjere i vidljivost, ne duplicirati C++ engine.

## 7. Vizualizacije

Vizualizacija pripada frontendu kada ne sudjeluje u matematičkom rezultatu.

- SVG je prikladan za likove, geometrijske oznake i diskretne dijagrame.
- Canvas je prikladan za mnogo uzoraka, kontinuirane grafove, pan i zoom.
- C++ treba vratiti matematičke točke/podatke, a JavaScript ih nacrtati.

Vizualizacija mora:

- reagirati na promjenu inputa
- imati tekstualni rezultat kao alternativu
- ne predstavljati aproksimativan crtež kao dokaz
- poštovati `prefers-reduced-motion`
- ostati čitljiva na mobilnoj širini

## 8. Zadaci

Ako se dodaje banka zadataka:

1. definirati stabilnu JSON shemu
2. koristiti jedinstvene ID-jeve
3. odvojiti očekivani rezultat od prikazne jedinice
4. definirati toleranciju za decimalni odgovor
5. dati trag i objašnjenje
6. verzionirati `localStorage` ključ ako se pohranjuje napredak
7. proširiti `scripts/validate_web.mjs`
8. dokumentirati shemu u [Zadaci i lokalna pohrana](exercises-and-storage.md)

Za simbolički odgovor prednost ima provjera kroz C++ parser/solver, a ne tekstualna jednakost.

## 9. Dokumentacija

Za novu aktivnu domenu ažurirati najmanje:

- `docs/README.md`
- `docs/core-domains.md`
- `docs/wasm-api.md` ako postoji export
- `docs/web-frontend.md` ako postoji UI
- `docs/testing-and-ci.md`
- korijenski `README.md`

Dokumentirati stvarni kod, ne planiranu funkcionalnost. Planirani modul jasno označiti kao placeholder.

## 10. Završna validacija

### C++ promjena

```powershell
cmake --build out/build/x64-debug
ctest --test-dir out/build/x64-debug --output-on-failure
```

### WASM promjena

```powershell
cmake --build out/build/wasm-release
```

Zatim napraviti Node smoke pozive.

### Web ili JSON promjena

```powershell
node scripts/validate_web.mjs
```

### Vizualna promjena

Pokrenuti lokalni HTTP server i ručno provjeriti desktop, mobilni prikaz, tipkovnicu, greške i konzolu.

## Primjer: aktivacija placeholder domene

Za `trigonometry`:

1. pročitati postojeće README ugovore
2. definirati npr. `Angles.hpp` i `Triangles.hpp` bez konflikta s Geometry imenima
3. implementirati u `core/src/trigonometry/`
4. dodati source u `core/CMakeLists.txt`
5. dodati GoogleTest datoteke
6. ažurirati README status domene
7. dodati WASM adapter ako je potreban webu
8. dodati `web/trigonometry.js` i HTML panel
9. povezati obrazovne razine
10. ažurirati `/docs` i root README
11. izvršiti cijeli validacijski niz

## Antipatterni

Izbjegavati:

- matematičke izračune implementirane samo u JavaScriptu
- jednu ogromnu klasu za nepovezane poddomene
- WASM adapter koji sadrži solver
- ručno uređivanje generiranog `aksiomat.js`
- dupliciranje enginea po obrazovnoj razini
- parsiranje JSON-a prije provjere greške
- testiranje samo preko browser UI-ja
- dodavanje source datoteke bez CMake registracije
- mijenjanje `localStorage` sheme bez verzije ili migracije
- dokumentaciju koja opisuje planirani kod kao već implementiran
