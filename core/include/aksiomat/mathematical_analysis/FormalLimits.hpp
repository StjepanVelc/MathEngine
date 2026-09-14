#pragma once

#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct EpsilonDeltaSample {
	double epsilon;
	double delta;
};

struct FormalLimitResult {
	std::string expression;
	double point;
	double limitValue;
	bool existsFinite;
	std::vector<EpsilonDeltaSample> epsilonDeltaTable;
	std::vector<std::string> steps;
};

struct ContinuityResult {
	std::string expression;
	double point;
	bool functionDefinedAtPoint;
	double functionValueAtPoint;
	double limitValue;
	bool isContinuous;
	std::vector<std::string> steps;
};

class FormalLimits {
public:
	// Formalna epsilon-delta provjera limesa polinoma kad x -> point.
	static FormalLimitResult evaluate(const std::string& expression, double point, const std::string& variable = "x");

	// Provjerava kontinuitet polinoma u tocki: f(point) definiran, limes postoji, i jednaki su.
	static ContinuityResult checkContinuity(const std::string& expression, double point, const std::string& variable = "x");
};

} // namespace aksiomat::mathematical_analysis
