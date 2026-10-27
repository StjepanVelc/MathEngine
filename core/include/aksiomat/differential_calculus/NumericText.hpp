#pragma once

#include <cmath>
#include <sstream>
#include <string>

namespace aksiomat::differential_calculus {

// Zapis broja za objasnjenja po koracima ("+inf", "-inf", "nedefinirano" za NaN).
inline std::string formatValue(double v) {
	if (std::isnan(v)) return "nedefinirano";
	if (std::isinf(v)) return v > 0 ? "+inf" : "-inf";
	std::ostringstream out;
	out.precision(8);
	out << (v == 0.0 ? 0.0 : v);
	return out.str();
}

// Zaokruzi numericki rezultat na "lijepu" vrijednost p/q (q <= 12) ako je dovoljno blizu.
inline double snapToNice(double value) {
	if (!std::isfinite(value)) return value;
	for (int q = 1; q <= 12; ++q) {
		const double p = std::round(value * q);
		if (std::fabs(value - p / q) <= 1e-4 * (1.0 + std::fabs(value))) return p / q + 0.0;
	}
	return value;
}

} // namespace aksiomat::differential_calculus
