#include "aksiomat/analytic_geometry/Circles.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::analytic_geometry {
namespace {

constexpr double tolerance = 1e-12;
void requireFinite(double value) { if (!std::isfinite(value)) throw std::invalid_argument("Circle data must be finite"); }
void requirePoint(Point2D point) { requireFinite(point.x); requireFinite(point.y); }
Circle requireCircle(Circle circle) { requirePoint(circle.center); requireFinite(circle.radius); if (circle.radius <= 0.0) throw std::invalid_argument("Circle radius must be positive"); return circle; }

} // namespace

Circle Circles::fromCenterRadius(Point2D center, double radius) {
	return requireCircle({center, radius});
}

Circle Circles::fromGeneralEquation(double d, double e, double f) {
	requireFinite(d); requireFinite(e); requireFinite(f);
	const Point2D center{-d / 2.0, -e / 2.0};
	const double squaredRadius = center.x * center.x + center.y * center.y - f;
	if (squaredRadius <= tolerance) throw std::invalid_argument("General equation does not define a real non-degenerate circle");
	return requireCircle({center, std::sqrt(squaredRadius)});
}

Circle Circles::throughThreePoints(Point2D first, Point2D second, Point2D third) {
	requirePoint(first); requirePoint(second); requirePoint(third);
	const double determinant = 2.0 * (first.x * (second.y - third.y) + second.x * (third.y - first.y) + third.x * (first.y - second.y));
	if (std::abs(determinant) <= tolerance) throw std::invalid_argument("Three non-collinear points are required");
	const double firstSquared = first.x * first.x + first.y * first.y;
	const double secondSquared = second.x * second.x + second.y * second.y;
	const double thirdSquared = third.x * third.x + third.y * third.y;
	const Point2D center{
		(firstSquared * (second.y - third.y) + secondSquared * (third.y - first.y) + thirdSquared * (first.y - second.y)) / determinant,
		(firstSquared * (third.x - second.x) + secondSquared * (first.x - third.x) + thirdSquared * (second.x - first.x)) / determinant};
	return requireCircle({center, std::hypot(first.x - center.x, first.y - center.y)});
}

CircleAnalysis Circles::analyze(Circle circle, std::size_t sampleCount) {
	circle = requireCircle(circle);
	if (sampleCount < 4 || sampleCount > 10000) throw std::invalid_argument("Circle sample count must be between 4 and 10000");
	std::vector<Point2D> samples; samples.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double angle = 2.0 * std::numbers::pi * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		samples.push_back({circle.center.x + circle.radius * std::cos(angle), circle.center.y + circle.radius * std::sin(angle)});
	}
	return {circle, 2.0 * std::numbers::pi * circle.radius, std::numbers::pi * circle.radius * circle.radius, std::move(samples)};
}

PointCirclePosition Circles::classify(Point2D point, Circle circle, double classificationTolerance) {
	requirePoint(point); circle = requireCircle(circle); requireFinite(classificationTolerance);
	if (classificationTolerance < 0.0) throw std::invalid_argument("Tolerance must be non-negative");
	const double difference = std::hypot(point.x - circle.center.x, point.y - circle.center.y) - circle.radius;
	if (std::abs(difference) <= classificationTolerance) return PointCirclePosition::On;
	return difference < 0.0 ? PointCirclePosition::Inside : PointCirclePosition::Outside;
}

} // namespace aksiomat::analytic_geometry
