#pragma once

#include <string>

namespace aksiomat::differential_calculus {

// JSON serijalizacija rezultata za web sloj. Nekonacni brojevi (NaN, +-inf) zapisuju se kao null,
// a limesi kao {"kind":"finite|plus_infinity|minus_infinity|does_not_exist|undefined","value":broj|null}.
// Sve funkcije bacaju std::invalid_argument za neispravan unos.
class DifferentialCalculusJson {
public:
	// pointText: broj ili "inf" / "+inf" / "-inf" (beskonacnost).
	static std::string limit(const std::string& expression, const std::string& pointText);
	static std::string derivative(const std::string& expression, int order);
	static std::string continuity(const std::string& expression, double point);
	static std::string tangent(const std::string& expression, double point);
	static std::string meanValue(const std::string& expression, double a, double b);
	static std::string study(const std::string& expression, double xMin, double xMax);
	static std::string sample(const std::string& expression, double xMin, double xMax, int count);
};

} // namespace aksiomat::differential_calculus
