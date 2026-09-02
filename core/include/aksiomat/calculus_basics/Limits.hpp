#pragma once

#include <string>
#include <vector>

namespace aksiomat::calculus_basics {

struct LimitApproachSample {
	double x;
	double value;
};

struct LimitResult {
	std::string expression;
	double point;
	double limitValue;
	bool existsFinite;
	std::vector<LimitApproachSample> leftSamples;
	std::vector<LimitApproachSample> rightSamples;
	std::vector<std::string> steps;
};

class Limits {
public:
	// Procjenjuje limes polinoma kad x -> point, numerickim priblizavanjem s obje strane.
	static LimitResult evaluate(const std::string& expression, double point, const std::string& variable = "x");
};

} // namespace aksiomat::calculus_basics
