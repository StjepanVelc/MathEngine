# Build i pokretanje

## Zahtjevi

Projekt trenutačno cilja:

- CMake najmanje 3.14
- C++20
- Ninja generator
- MSVC za Windows native build
- Emscripten SDK za WebAssembly build
- Node.js za web validaciju i opcionalne WASM smoke testove
- Python ili drugi statički HTTP server za lokalno pokretanje weba

Preporučeno Windows okruženje je Visual Studio Developer PowerShell jer osigurava dostupnost `cl.exe` i pripadajućih SDK varijabli.

## CMake struktura

Korijenski `CMakeLists.txt` uključuje `core` u svim konfiguracijama.

Native konfiguracija dodaje:

- `app`
- `tests`

Emscripten konfiguracija preskače native aplikaciju i GoogleTest te u `core/CMakeLists.txt` gradi `aksiomat_wasm`.

## Preseti

`CMakePresets.json` definira:

| Preset | Arhitektura | Konfiguracija | Toolchain |
|---|---|---|---|
| `x64-debug` | x64 | Debug | MSVC |
| `x64-release` | x64 | Release | MSVC |
| `x86-debug` | x86 | Debug | MSVC |
| `x86-release` | x86 | Release | MSVC |
| `wasm-release` | WebAssembly | Release | Emscripten |

Binary direktorij svakog preseta je `out/build/<preset>`.

## Native Debug build

Iz korijena repozitorija:

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Prva naredba konfigurira projekt. Druga pokreće Ninja build nad već konfiguriranim direktorijem.

Glavni izlazi:

```text
out/build/x64-debug/core/aksiomat_core.lib
out/build/x64-debug/app/mathengine_app.exe
out/build/x64-debug/tests/core_tests.exe
```

Točna ekstenzija i raspored mogu ovisiti o platformi i generatoru, ali navedene putanje odgovaraju trenutačnom Windows/Ninja okruženju.

## Native Release build

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release
```

Release koristi optimizacije i `NDEBUG`.

## x86 build

```powershell
cmake --preset x86-debug
cmake --build out/build/x86-debug
```

Koristiti samo kada je potreban 32-bitni artefakt. x64 je zadani razvojni cilj.

## Pokretanje native aplikacije

Nakon Debug builda:

```powershell
.\out\build\x64-debug\app\mathengine_app.exe
```

Native aplikacija je demonstracijski klijent jezgre; puna edukacijska funkcionalnost nalazi se u web sučelju.

## Emscripten priprema

Emscripten SDK mora biti instaliran i aktiviran. Tipičan postupak:

```powershell
cd C:\putanja\do\emsdk
.\emsdk install latest
.\emsdk activate latest
.\emsdk_env.ps1
```

Nakon aktivacije varijabla `EMSDK` mora pokazivati na korijen SDK-a, a `em++` mora biti dostupan.

Provjera:

```powershell
em++ --version
$env:EMSDK
```

## WebAssembly build

Iz korijena MathEngine repozitorija:

```powershell
cmake --preset wasm-release
cmake --build out/build/wasm-release
```

Preset postavlja Emscripten toolchain datoteku:

```text
$env{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
```

Cilj `aksiomat_wasm` povezuje se sa statičkom bibliotekom `aksiomat_core`.

### Emscripten opcije

- `-sMODULARIZE=1` — loader vraća tvorničku funkciju
- `-sEXPORT_NAME=createAksiomatModule` — ime tvorničke funkcije
- `--bind` — uključuje Embind
- `-fexceptions` — omogućuje hvatanje C++ iznimki u adapteru

### Artefakti

Build proizvodi:

```text
out/build/wasm-release/core/aksiomat.js
out/build/wasm-release/core/aksiomat.wasm
```

Post-build naredba ih kopira u:

```text
web/aksiomat.js
web/aksiomat.wasm
```

Te dvije web datoteke ne uređuju se ručno.

## Kada ponovno graditi WASM

WASM build je obavezan nakon promjene:

- bilo kojeg aktivnog C++ zaglavlja ili izvora
- `core/src/wasm_bindings.cpp`
- `core/CMakeLists.txt` ako se dodaje novi source
- Emscripten link opcija

Promjena samo HTML-a, CSS-a, JavaScripta ili JSON zadataka ne zahtijeva WASM rekompilaciju.

## Pokretanje web aplikacije

Web aplikacija koristi `fetch` za WASM i JSON, pa je treba servirati HTTP-om.

### Python

```powershell
cd web
python -m http.server 8000
```

Otvoriti:

```text
http://localhost:8000
```

### Alternativni server

Može se koristiti bilo koji statički server koji pravilno poslužuje `.js`, `.wasm`, `.json`, `.html` i `.css`.

Za `.wasm` je preporučen MIME tip:

```text
application/wasm
```

Emscripten loader može imati fallback ponašanje, ali ispravan MIME tip daje pouzdanije i učinkovitije učitavanje.

## Čisti rebuild

Ako konfiguracija postane nekonzistentna, ukloniti samo odgovarajući build direktorij i ponovno konfigurirati:

```powershell
Remove-Item -Recurse -Force out/build/x64-debug
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Za WASM:

```powershell
Remove-Item -Recurse -Force out/build/wasm-release
cmake --preset wasm-release
cmake --build out/build/wasm-release
```

Ne brisati ručno izvorne datoteke u `web/`; generirani `aksiomat.js/.wasm` bit će ponovno kopirani.

## Česte poteškoće

### `cl.exe` nije pronađen

Pokrenuti naredbe iz Visual Studio Developer PowerShella ili ponovno konfigurirati nakon aktivacije MSVC okruženja.

### `EMSDK` nije postavljen

Pokrenuti `emsdk_env.ps1` u trenutnoj terminal sesiji prije `cmake --preset wasm-release`.

### CMake upozorenje o Emscripten shared libraries

Trenutačna konfiguracija gradi statičku jezgru i Emscripten izvršni modul. Upozorenje specifične CMake/Emscripten kombinacije nije nužno kvar ako konfiguracija i link završe uspješno. Ipak, stvarni završni status builda mora biti uspješan.

### Web prikazuje da WASM modul nije pronađen

Provjeriti:

1. postoje li `web/aksiomat.js` i `web/aksiomat.wasm`
2. je li završen WASM build
3. otvara li se aplikacija preko HTTP servera
4. prikazuje li konzola preglednika 404 ili MIME grešku

### JSON zadaci se ne učitavaju

Provjeriti mrežni zahtjev prema `data/<banka>.json` i ne koristiti `file://`.

### `ninja: no work to do`

To je uspješan rezultat: svi artefakti su noviji od izvora i nema potrebe za rekompilacijom.

## Preporučeni razvojni ciklus

### Promjena samo frontenda

```powershell
node scripts/validate_web.mjs
cd web
python -m http.server 8000
```

### Promjena C++ jezgre

```powershell
cmake --build out/build/x64-debug
ctest --test-dir out/build/x64-debug --output-on-failure
cmake --build out/build/wasm-release
node scripts/validate_web.mjs
```

### Nova datoteka ili promjena CMakea

Ponovno pokrenuti configure preset prije builda ako Ninja automatska regeneracija nije dovoljna:

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
```
