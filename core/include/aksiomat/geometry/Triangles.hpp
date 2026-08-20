#pragma once

#include <string>

namespace aksiomat::geometry {

struct TriangleClassification {
	std::string bySides;
	std::string byAngles;
};

class Triangles {
public:
	static TriangleClassification classify(double sideA, double sideB, double sideC);
	static double thirdAngle(double angleA, double angleB);
	static double hypotenuse(double legA, double legB);
	static double leg(double hypotenuse, double knownLeg);
};

} // namespace aksiomat::geometry
