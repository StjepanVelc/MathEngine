# MathEngine obrazovni roadmap

Ovaj dokument prati redoslijed razvoja matematičkih modula i njihovu primarnu obrazovnu razinu. Status domene opisuje implementaciju, dok odsjek opisuje kome je trenutačni sadržaj namijenjen.

## Oznake statusa

- **Riješeno** — C++20 domena, testovi, WASM, web sučelje i dokumentacija postoje.
- **U izradi** — aktivni razvoj domene.
- **Planirano** — opseg je dogovoren, ali implementacija nije završena.
- **Kasnije** — proširenje nakon prve javne verzije.

## Srednja škola

| Redoslijed | Modul | Status | Planirani sadržaj |
|---:|---|---|---|
| 1 | Nizovi i redovi | **Riješeno** | Članovi i monotonost, aritmetički/geometrijski nizovi i redovi, rekurzije, Fibonacci, kamate, rast i amortizacija |
| 2 | Analitička geometrija | **Riješeno** | Točke, vektori, pravci, udaljenosti, kružnica, parabola, elipsa i hiperbola u ravnini |
| 3 | Eksponencijalne i logaritamske funkcije | **Riješeno** | Potencije i korijeni, eksponencijalni rast/pad, logaritmi, jednadžbe, grafovi i primjene |
| 4 | Kombinatorika, vjerojatnost i statistika | **Riješeno** | Pravila prebrojavanja, permutacije, varijacije, kombinacije, vjerojatnost, deskriptivna statistika i vizualizacije podataka |
| 5 | Matematička analiza — srednjoškolske osnove | **Riješeno** | Intuitivni limes, derivacija kao brzina promjene, osnovna pravila deriviranja, primjene derivacije i određeni integral kao površina |

## Fakultet i napredno

Ovaj odsjek ne duplicira srednjoškolske module. Proširuje istu C++ jezgru formalnijim definicijama, općenitijim strukturama i naprednijim prikazima.

| Modul | Status | Planirano proširenje |
|---|---|---|
| Matematička analiza | **Kasnije** | Formalni limesi i kontinuitet, napredne derivacije i integrali, redovi funkcija, funkcije više varijabli i diferencijalne jednadžbe |
| Analitička geometrija i linearna algebra | **Kasnije** | Vektori i ravnine u 3D, opći kvadrici, matrice transformacija, svojstvene vrijednosti i vektorski prostori |
| Vjerojatnost i statistika | **Kasnije** | Slučajne varijable, distribucije, očekivanje/varijanca, intervali pouzdanosti, testiranje hipoteza i regresija |
| Diskretna matematika | **Kasnije** | Skupovi, relacije, teorija grafova, rekurencije i napredna kombinatorika |
| Kompleksni brojevi | **Kasnije** | Algebarski, trigonometrijski i eksponencijalni oblik te primjene u jednadžbama i geometriji |

## Pravila razvoja

1. Matematička jezgra ostaje neovisna o obrazovnoj razini.
2. Svaka aktivna domena ima zasebne `.hpp/.cpp` datoteke, GoogleTest pokriće i tanke WASM adaptere.
3. Web sloj određuje jezik objašnjenja, primjere, vizualizacije i razinu težine.
4. Novi modul završava tek kada prođu native testovi, WASM smoke test i web validator.
5. Status u ovom dokumentu ažurira se nakon završne validacije modula.
