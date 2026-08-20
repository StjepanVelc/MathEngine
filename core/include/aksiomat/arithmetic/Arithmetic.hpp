#pragma once

#include <string>

namespace aksiomat {

// Kompatibilna fasada naprednog aritmetičkog parsera i evaluatora.
// Podržava +, -, *, /, %, ^, !, zagrade, znanstveni zapis, pi/e te
// sqrt, abs, min, max, round, floor, ceil i mod.
class Arithmetic {
public:
	static double evaluate(const std::string& expression);
};

} // namespace aksiomat
