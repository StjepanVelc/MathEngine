#pragma once

#include <string>

namespace aksiomat {

// Aritmetički evaluator izraza.
// Podržava: +, -, *, /, zagrade, decimalne brojeve, unarni minus.
// Baca std::invalid_argument za neispravan izraz i dijeljenje s nulom.
class Arithmetic {
public:
	static double evaluate(const std::string& expression);
};

} // namespace aksiomat
