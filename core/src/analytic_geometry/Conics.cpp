#include "aksiomat/analytic_geometry/Conics.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::analytic_geometry {
namespace {

void requireFinite(double value) { if (!std::isfinite(value)) throw std::invalid_argument("Conic data must be finite"); }
void requirePoint(Point2D point) { requireFinite(point.x); requireFinite(point.y); }
void requirePositive(double value, const char* message) { requireFinite(value); if (value <= 0.0) throw std::invalid_argument(message); }
void requireSamples(std::size_t sampleCount) { if (sampleCount < 4 || sampleCount > 10000) throw std::invalid_argument("Conic sample count must be between 4 and 10000"); }

} // namespace

ParabolaAnalysis Conics::analyze(Parabola parabola, double extent, std::size_t sampleCount) {
	requirePoint(parabola.vertex); requireFinite(parabola.parameter); requirePositive(extent, "Parabola extent must be positive"); requireSamples(sampleCount);
	if (parabola.parameter == 0.0) throw std::invalid_argument("Parabola parameter cannot be zero");
	Point2D focus; double directrix;
	if (parabola.orientation == ParabolaOrientation::Vertical) {
		focus = {parabola.vertex.x, parabola.vertex.y + parabola.parameter}; directrix = parabola.vertex.y - parabola.parameter;
	} else if (parabola.orientation == ParabolaOrientation::Horizontal) {
		focus = {parabola.vertex.x + parabola.parameter, parabola.vertex.y}; directrix = parabola.vertex.x - parabola.parameter;
	} else throw std::invalid_argument("Unknown parabola orientation");
	std::vector<Point2D> samples; samples.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double parameter = -extent + 2.0 * extent * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		if (parabola.orientation == ParabolaOrientation::Vertical) samples.push_back({parabola.vertex.x + parameter, parabola.vertex.y + parameter * parameter / (4.0 * parabola.parameter)});
		else samples.push_back({parabola.vertex.x + parameter * parameter / (4.0 * parabola.parameter), parabola.vertex.y + parameter});
	}
	return {parabola, focus, directrix, std::move(samples)};
}

EllipseAnalysis Conics::analyze(Ellipse ellipse, std::size_t sampleCount) {
	requirePoint(ellipse.center); requirePositive(ellipse.semiMajor, "Ellipse semi-major axis must be positive"); requirePositive(ellipse.semiMinor, "Ellipse semi-minor axis must be positive"); requireSamples(sampleCount);
	if (ellipse.semiMajor < ellipse.semiMinor) throw std::invalid_argument("Ellipse semi-major axis must not be smaller than semi-minor axis");
	const double focalDistance = std::sqrt(ellipse.semiMajor * ellipse.semiMajor - ellipse.semiMinor * ellipse.semiMinor);
	const Point2D focus1 = ellipse.horizontalMajorAxis ? Point2D{ellipse.center.x - focalDistance, ellipse.center.y} : Point2D{ellipse.center.x, ellipse.center.y - focalDistance};
	const Point2D focus2 = ellipse.horizontalMajorAxis ? Point2D{ellipse.center.x + focalDistance, ellipse.center.y} : Point2D{ellipse.center.x, ellipse.center.y + focalDistance};
	std::vector<Point2D> samples; samples.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double angle = 2.0 * std::numbers::pi * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		const double major = ellipse.semiMajor * std::cos(angle), minor = ellipse.semiMinor * std::sin(angle);
		samples.push_back(ellipse.horizontalMajorAxis ? Point2D{ellipse.center.x + major, ellipse.center.y + minor} : Point2D{ellipse.center.x + minor, ellipse.center.y + major});
	}
	return {ellipse, focalDistance, focalDistance / ellipse.semiMajor, focus1, focus2, std::move(samples)};
}

HyperbolaAnalysis Conics::analyze(Hyperbola hyperbola, double extent, std::size_t sampleCount) {
	requirePoint(hyperbola.center); requirePositive(hyperbola.transverse, "Hyperbola transverse semi-axis must be positive"); requirePositive(hyperbola.conjugate, "Hyperbola conjugate semi-axis must be positive"); requirePositive(extent, "Hyperbola extent must be positive"); requireSamples(sampleCount);
	const double focalDistance = std::hypot(hyperbola.transverse, hyperbola.conjugate);
	const Point2D focus1 = hyperbola.horizontalTransverseAxis ? Point2D{hyperbola.center.x - focalDistance, hyperbola.center.y} : Point2D{hyperbola.center.x, hyperbola.center.y - focalDistance};
	const Point2D focus2 = hyperbola.horizontalTransverseAxis ? Point2D{hyperbola.center.x + focalDistance, hyperbola.center.y} : Point2D{hyperbola.center.x, hyperbola.center.y + focalDistance};
	std::vector<Point2D> branch1, branch2; branch1.reserve(sampleCount); branch2.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double parameter = -extent + 2.0 * extent * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		const double transverse = hyperbola.transverse * std::cosh(parameter), conjugate = hyperbola.conjugate * std::sinh(parameter);
		if (hyperbola.horizontalTransverseAxis) { branch1.push_back({hyperbola.center.x + transverse, hyperbola.center.y + conjugate}); branch2.push_back({hyperbola.center.x - transverse, hyperbola.center.y + conjugate}); }
		else { branch1.push_back({hyperbola.center.x + conjugate, hyperbola.center.y + transverse}); branch2.push_back({hyperbola.center.x + conjugate, hyperbola.center.y - transverse}); }
	}
	return {hyperbola, focalDistance, focalDistance / hyperbola.transverse, focus1, focus2, std::move(branch1), std::move(branch2)};
}

} // namespace aksiomat::analytic_geometry
