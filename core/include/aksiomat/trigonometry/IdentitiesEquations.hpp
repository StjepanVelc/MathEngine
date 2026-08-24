#pragma once

#include <string>
#include <vector>

#include "aksiomat/trigonometry/TrigFunctions.hpp"

namespace aksiomat::trigonometry {

enum class TrigIdentity {
	Pythagorean,
	TangentRatio,
	DoubleAngleSine,
	DoubleAngleCosine
};

struct IdentityCheck {
	bool holds;
	double left;
	double right;
	std::string formula;
};

class IdentitiesEquations {
public:
	static IdentityCheck check(TrigIdentity identity, double angleDegrees);
	static std::vector<double> solve(TrigFunction function, double value, double minDegrees, double maxDegrees);
};

} // namespace aksiomat::trigonometry
