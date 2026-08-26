#pragma once

#include <cstddef>
#include <vector>

#include "aksiomat/analytic_geometry/PointsVectors.hpp"

namespace aksiomat::analytic_geometry {

enum class ParabolaOrientation {
	Horizontal,
	Vertical
};

struct Parabola {
	Point2D vertex;
	double parameter;
	ParabolaOrientation orientation;
};

struct ParabolaAnalysis {
	Parabola parabola;
	Point2D focus;
	double directrix;
	std::vector<Point2D> samples;
};

struct Ellipse {
	Point2D center;
	double semiMajor;
	double semiMinor;
	bool horizontalMajorAxis;
};

struct EllipseAnalysis {
	Ellipse ellipse;
	double focalDistance;
	double eccentricity;
	Point2D focus1;
	Point2D focus2;
	std::vector<Point2D> samples;
};

struct Hyperbola {
	Point2D center;
	double transverse;
	double conjugate;
	bool horizontalTransverseAxis;
};

struct HyperbolaAnalysis {
	Hyperbola hyperbola;
	double focalDistance;
	double eccentricity;
	Point2D focus1;
	Point2D focus2;
	std::vector<Point2D> branch1;
	std::vector<Point2D> branch2;
};

class Conics {
public:
	static ParabolaAnalysis analyze(Parabola parabola, double extent = 10.0, std::size_t sampleCount = 121);
	static EllipseAnalysis analyze(Ellipse ellipse, std::size_t sampleCount = 181);
	static HyperbolaAnalysis analyze(Hyperbola hyperbola, double extent = 3.0, std::size_t sampleCount = 121);
};

} // namespace aksiomat::analytic_geometry
