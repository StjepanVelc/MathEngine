#include "aksiomat/analytic_geometry/LinesDistances.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::analytic_geometry {
namespace {

constexpr double tolerance = 1e-12;

void requireFinite(double value) { if (!std::isfinite(value)) throw std::invalid_argument("Line data must be finite"); }
void requireFinite(Point2D point) { requireFinite(point.x); requireFinite(point.y); }

Line2D normalized(Line2D line) {
	requireFinite(line.a); requireFinite(line.b); requireFinite(line.c);
	const double norm = std::hypot(line.a, line.b);
	if (norm <= tolerance) throw std::invalid_argument("Line normal vector cannot be zero");
	line.a /= norm; line.b /= norm; line.c /= norm;
	if (line.a < -tolerance || (std::abs(line.a) <= tolerance && line.b < 0.0)) {
		line.a = -line.a; line.b = -line.b; line.c = -line.c;
	}
	if (std::abs(line.a) <= tolerance) line.a = 0.0;
	if (std::abs(line.b) <= tolerance) line.b = 0.0;
	if (std::abs(line.c) <= tolerance) line.c = 0.0;
	return line;
}

} // namespace

Line2D LinesDistances::throughPoints(Point2D first, Point2D second) {
	requireFinite(first); requireFinite(second);
	if (std::hypot(second.x - first.x, second.y - first.y) <= tolerance) throw std::invalid_argument("Two distinct points are required");
	return normalized({first.y - second.y, second.x - first.x, first.x * second.y - second.x * first.y});
}

Line2D LinesDistances::pointDirection(Point2D point, Vector2D direction) {
	requireFinite(point); requireFinite(direction.x); requireFinite(direction.y);
	if (std::hypot(direction.x, direction.y) <= tolerance) throw std::invalid_argument("Direction vector cannot be zero");
	return normalized({-direction.y, direction.x, direction.y * point.x - direction.x * point.y});
}

Line2D LinesDistances::fromSlopeIntercept(double slope, double intercept) {
	requireFinite(slope); requireFinite(intercept);
	return normalized({slope, -1.0, intercept});
}

Line2D LinesDistances::parallelThrough(Line2D line, Point2D point) {
	line = normalized(line); requireFinite(point);
	return normalized({line.a, line.b, -(line.a * point.x + line.b * point.y)});
}

Line2D LinesDistances::perpendicularThrough(Line2D line, Point2D point) {
	line = normalized(line); requireFinite(point);
	return normalized({line.b, -line.a, -(line.b * point.x - line.a * point.y)});
}

LineAnalysis LinesDistances::analyze(Line2D line) {
	line = normalized(line);
	const bool vertical = std::abs(line.b) <= tolerance;
	return {line, vertical,
		vertical ? std::nullopt : std::optional<double>{-line.a / line.b},
		std::abs(line.a) <= tolerance ? std::nullopt : std::optional<double>{-line.c / line.a},
		vertical ? std::nullopt : std::optional<double>{-line.c / line.b}};
}

LinesRelationResult LinesDistances::relation(Line2D first, Line2D second) {
	first = normalized(first); second = normalized(second);
	const double determinant = first.a * second.b - second.a * first.b;
	const double dotNormals = first.a * second.a + first.b * second.b;
	if (std::abs(determinant) <= tolerance) {
		const bool coincident = std::abs(first.c - second.c) <= tolerance;
		return {coincident ? LineRelation::Coincident : LineRelation::Parallel, std::nullopt, 0.0};
	}
	const Point2D intersection{(first.b * second.c - second.b * first.c) / determinant,
		(first.c * second.a - second.c * first.a) / determinant};
	double angle = std::acos(std::clamp(std::abs(dotNormals), 0.0, 1.0)) * 180.0 / std::numbers::pi;
	if (std::abs(dotNormals) <= tolerance) angle = 90.0;
	return {angle == 90.0 ? LineRelation::Perpendicular : LineRelation::Intersecting, intersection, angle};
}

double LinesDistances::pointDistance(Point2D first, Point2D second) {
	requireFinite(first); requireFinite(second);
	return std::hypot(second.x - first.x, second.y - first.y);
}

double LinesDistances::pointToLineDistance(Point2D point, Line2D line) {
	requireFinite(point); line = normalized(line);
	return std::abs(line.a * point.x + line.b * point.y + line.c);
}

} // namespace aksiomat::analytic_geometry
