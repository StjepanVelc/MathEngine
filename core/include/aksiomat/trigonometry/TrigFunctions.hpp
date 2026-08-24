#pragma once

#include <string>

#include "aksiomat/trigonometry/Angles.hpp"

namespace aksiomat::trigonometry {

enum class TrigFunction {
	Sine,
	Cosine,
	Tangent
};

struct TrigValue {
	double value;
	std::string exact;
	bool hasExactForm;
};

class TrigFunctions {
public:
	static TrigValue evaluate(TrigFunction function, double angle, AngleUnit unit = AngleUnit::Degrees);
	static double inverse(TrigFunction function, double value, AngleUnit resultUnit = AngleUnit::Degrees);
};

} // namespace aksiomat::trigonometry
