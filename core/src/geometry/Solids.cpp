#include "aksiomat/geometry/Solids.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::geometry {
namespace {

void requirePositive(double value) {
	if (!std::isfinite(value) || value <= 0.0) {
		throw std::invalid_argument("Mjera mora biti konacan pozitivan broj");
	}
}

} // namespace

SolidResult Solids::cube(double side) {
	requirePositive(side);
	return {6.0 * side * side, side * side * side};
}

SolidResult Solids::cuboid(double length, double width, double height) {
	requirePositive(length);
	requirePositive(width);
	requirePositive(height);
	return {2.0 * (length * width + length * height + width * height), length * width * height};
}

SolidResult Solids::prism(double baseArea, double basePerimeter, double height) {
	requirePositive(baseArea);
	requirePositive(basePerimeter);
	requirePositive(height);
	return {2.0 * baseArea + basePerimeter * height, baseArea * height};
}

SolidResult Solids::cylinder(double radius, double height) {
	requirePositive(radius);
	requirePositive(height);
	return {2.0 * std::numbers::pi * radius * (radius + height), std::numbers::pi * radius * radius * height};
}

} // namespace aksiomat::geometry
