# linear algebra

Javni C++ ugovori za domenu. Linearna algebra: vektori i ravnine u prostoru, kvadrike, matrice (determinanta, inverz, množenje, transformacije), svojstvene vrijednosti simetričnih matrica 2x2/3x3 te provjera linearne nezavisnosti i baze.

Ova mapa ne pripada jednoj obrazovnoj razini: isti API može koristiti srednjoškolski i napredni frontend s različitim primjerima i objašnjenjima.

Domena je implementirana, registrirana u `core/CMakeLists.txt`, pokrivena GoogleTest testovima (`tests/test_linear_algebra_*.cpp`) i izložena kroz WASM adaptere `linearAlgebra*` u `core/src/wasm_bindings.cpp` (koristi ih `web/analytic-algebra.js`).
