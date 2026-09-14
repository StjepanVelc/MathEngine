# complex numbers

Javni C++ ugovori za domenu. Kompleksni brojevi: osnovne operacije, konjugacija, pretvorba između algebarskog i trigonometrijskog (polarnog) oblika, potenciranje, n-ti korijeni i rješavanje kvadratnih jednadžbi s kompleksnim rješenjima.

Ova mapa ne pripada jednoj obrazovnoj razini: isti API može koristiti srednjoškolski i napredni frontend s različitim primjerima i objašnjenjima.

Domena je implementirana, registrirana u `core/CMakeLists.txt`, pokrivena GoogleTest testovima (`tests/test_complex_numbers.cpp`, `tests/test_complex_applications.cpp`) i izložena kroz WASM adaptere `complexNumbers*` u `core/src/wasm_bindings.cpp` (koristi ih `web/complex-numbers.js`).
