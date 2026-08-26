#pragma once

#include <vector>

#include "aksiomat/analytic_geometry/PointsVectors.hpp"

namespace aksiomat::analytic_geometry {

enum class PointCirclePosition {
	Inside,
	On,
	Outside
};

struct Circle {
	Point2D center;
	double radius;
};

struct CircleAnalysis {
	Circle circle;
	double circumference;
	double area;
	std::vector<Point2D> samples;
};

class Circles {
public:
	static Circle fromCenterRadius(Point2D center, double radius);
	static Circle fromGeneralEquation(double d, double e, double f);
	static Circle throughThreePoints(Point2D first, Point2D second, Point2D third);
	static CircleAnalysis analyze(Circle circle, std::size_t sampleCount = 121);
	static PointCirclePosition classify(Point2D point, Circle circle, double tolerance = 1e-9);
};

} // namespace aksiomat::analytic_geometry
