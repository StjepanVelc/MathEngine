#pragma once

#include <string>
#include <vector>

namespace aksiomat::calculus_basics {

struct DerivativeResult {
	std::string original;
	std::string derivative;
	double pointValue;
	double slopeAtPoint;
	std::string tangentLine;
	std::vector<std::string> steps;
};

struct RateOfChangeResult {
	double averageRate;
	double instantaneousRate;
	std::vector<std::string> steps;
};

class Derivatives {
public:
	// Deriviraj polinom i izracunaj nagib tangente u zadanoj tocki.
	static DerivativeResult differentiate(const std::string& expression, double point, const std::string& variable = "x");

	// Usporeduje prosjecnu brzinu promjene na [a,b] s trenutnom brzinom promjene u tocki a (derivacija).
	static RateOfChangeResult rateOfChange(const std::string& expression, double a, double b, const std::string& variable = "x");
};

} // namespace aksiomat::calculus_basics
