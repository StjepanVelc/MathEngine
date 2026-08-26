#pragma once

#include <optional>

#include "aksiomat/analytic_geometry/PointsVectors.hpp"

namespace aksiomat::analytic_geometry {

enum class LineRelation {
	Intersecting,
	Parallel,
	Coincident,
	Perpendicular
};

struct Line2D {
	double a;
	double b;
	double c;
};

struct LineAnalysis {
	Line2D line;
	bool vertical;
	std::optional<double> slope;
	std::optional<double> xIntercept;
	std::optional<double> yIntercept;
};

struct LinesRelationResult {
	LineRelation relation;
	std::optional<Point2D> intersection;
	double acuteAngleDegrees;
};

class LinesDistances {
public:
	static Line2D throughPoints(Point2D first, Point2D second);
	static Line2D pointDirection(Point2D point, Vector2D direction);
	static Line2D fromSlopeIntercept(double slope, double intercept);
	static Line2D parallelThrough(Line2D line, Point2D point);
	static Line2D perpendicularThrough(Line2D line, Point2D point);
	static LineAnalysis analyze(Line2D line);
	static LinesRelationResult relation(Line2D first, Line2D second);
	static double pointDistance(Point2D first, Point2D second);
	static double pointToLineDistance(Point2D point, Line2D line);
};

} // namespace aksiomat::analytic_geometry
