# discrete math

Javni C++ ugovori za domenu. Diskretna matematika: operacije nad skupovima, svojstva relacija, teorija grafova (analiza, najkraći put), linearne rekurencije te napredna kombinatorika (uključivanje-isključivanje, Dirichletov princip, deranžmani).

Ova mapa ne pripada jednoj obrazovnoj razini: isti API može koristiti srednjoškolski i napredni frontend s različitim primjerima i objašnjenjima.

Domena je implementirana, registrirana u `core/CMakeLists.txt`, pokrivena GoogleTest testovima (`tests/test_discrete_math_*.cpp`) i izložena kroz WASM adaptere `discreteMath*` u `core/src/wasm_bindings.cpp` (koristi ih `web/discrete-math.js`).
