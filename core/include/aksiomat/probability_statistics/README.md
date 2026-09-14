# probability statistics

Javni C++ ugovori za domenu. Fakultetsko proširenje vjerojatnosti i statistike: binomna, Poissonova, normalna i uniformna distribucija te statistička inferencija (intervali pouzdanosti, z-test, linearna regresija).

Ova mapa ne pripada jednoj obrazovnoj razini, ali je odvojena od srednjoškolskog modula `combinatorics_probability_statistics`: isti API može koristiti napredni frontend s drugačijim primjerima i objašnjenjima.

Domena je implementirana, registrirana u `core/CMakeLists.txt`, pokrivena GoogleTest testovima (`tests/test_probability_statistics_*.cpp`) i izložena kroz WASM adaptere `probabilityStatistics*` u `core/src/wasm_bindings.cpp` (koristi ih `web/probability-statistics.js`).
