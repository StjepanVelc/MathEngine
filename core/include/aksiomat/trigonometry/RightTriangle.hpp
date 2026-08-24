#pragma once

#include <string>
#include <vector>

namespace aksiomat::trigonometry {

enum class KnownSide {
	Opposite,
	Adjacent,
	Hypotenuse
};

struct RightTriangleSolution {
	double opposite;
	double adjacent;
	double hypotenuse;
	double angleDegrees;
	double complementaryAngleDegrees;
	std::vector<std::string> steps;
};

class RightTriangle {
public:
	static RightTriangleSolution fromLegs(double opposite, double adjacent);
	static RightTriangleSolution fromLegAndHypotenuse(double leg, double hypotenuse, KnownSide legType);
	static RightTriangleSolution fromSideAndAngle(double side, KnownSide sideType, double angleDegrees);
};

} // namespace aksiomat::trigonometry
