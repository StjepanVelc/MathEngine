#include "aksiomat/trigonometry/Angles.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::trigonometry {
namespace {

constexpr double axisTolerance = 1e-12;

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Kut mora biti konacan broj");
}

double cleanAxisValue(double value) {
	return std::abs(value) < axisTolerance ? 0.0 : value;
}

int quadrantFor(double degrees) {
	if (degrees == 0.0 || degrees == 90.0 || degrees == 180.0 || degrees == 270.0) return 0;
	if (degrees < 90.0) return 1;
	if (degrees < 180.0) return 2;
	if (degrees < 270.0) return 3;
	return 4;
}

double referenceFor(double degrees) {
	if (degrees <= 90.0) return degrees;
	if (degrees <= 180.0) return 180.0 - degrees;
	if (degrees <= 270.0) return degrees - 180.0;
	return 360.0 - degrees;
}

} // namespace

double Angles::toRadians(double degrees) {
	requireFinite(degrees);
	return degrees * std::numbers::pi / 180.0;
}

double Angles::toDegrees(double radians) {
	requireFinite(radians);
	return radians * 180.0 / std::numbers::pi;
}

double Angles::normalizeDegrees(double degrees) {
	requireFinite(degrees);
	double normalized = std::fmod(degrees, 360.0);
	if (normalized < 0.0) normalized += 360.0;
	return normalized == 0.0 ? 0.0 : normalized;
}

double Angles::normalizeRadians(double radians) {
	requireFinite(radians);
	const double fullTurn = 2.0 * std::numbers::pi;
	double normalized = std::fmod(radians, fullTurn);
	if (normalized < 0.0) normalized += fullTurn;
	return normalized == 0.0 ? 0.0 : normalized;
}

UnitCirclePoint Angles::unitCircle(double angle, AngleUnit unit) {
	requireFinite(angle);
	if (unit != AngleUnit::Degrees && unit != AngleUnit::Radians) {
		throw std::invalid_argument("Nepoznata jedinica kuta");
	}
	const double degrees = normalizeDegrees(unit == AngleUnit::Degrees ? angle : toDegrees(angle));
	const double radians = toRadians(degrees);
	return {
		degrees,
		radians,
		cleanAxisValue(std::cos(radians)),
		cleanAxisValue(std::sin(radians)),
		quadrantFor(degrees),
		referenceFor(degrees)
	};
}

} // namespace aksiomat::trigonometry
