#pragma once

#include <string>
#include <vector>

namespace aksiomat::trigonometry {

struct GeneralTriangleSolution {
	double sideA;
	double sideB;
	double sideC;
	double angleA;
	double angleB;
	double angleC;
	double area;
	std::vector<std::string> steps;
};

class GeneralTriangle {
public:
	static GeneralTriangleSolution fromSides(double sideA, double sideB, double sideC);
	static GeneralTriangleSolution fromTwoSidesAndIncludedAngle(double sideA, double sideB, double includedAngleC);
	static GeneralTriangleSolution fromSideAndTwoAngles(double sideA, double angleA, double angleB);
};

} // namespace aksiomat::trigonometry
