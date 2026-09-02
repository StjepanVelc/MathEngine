#pragma once

#include <string>
#include <vector>

namespace aksiomat::calculus_basics {

struct CriticalPoint {
	double x;
	double y;
	std::string kind; // "minimum", "maksimum" ili "prijevojna tocka"
};

struct DerivativeApplicationResult {
	std::string expression;
	std::string derivative;
	std::vector<CriticalPoint> criticalPoints;
	std::vector<std::string> increasingIntervals;
	std::vector<std::string> decreasingIntervals;
	std::vector<std::string> steps;
};

class DerivativeApplications {
public:
	// Analizira monotonost i lokalne ekstreme polinoma (podrzava izvod stupnja do 2, tj. izvorni polinom do stupnja 3).
	static DerivativeApplicationResult analyze(const std::string& expression, const std::string& variable = "x");
};

} // namespace aksiomat::calculus_basics
