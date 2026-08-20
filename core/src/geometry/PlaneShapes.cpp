#include "aksiomat/geometry/PlaneShapes.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::geometry {
namespace {

void requirePositive(double value) {
	if (!std::isfinite(value) || value <= 0.0) {
		throw std::invalid_argument("Duljina mora biti konacan pozitivan broj");
	}
}

void requireTriangle(double sideA, double sideB, double sideC) {
	requirePositive(sideA);
	requirePositive(sideB);
	requirePositive(sideC);
	if (sideA + sideB <= sideC || sideA + sideC <= sideB || sideB + sideC <= sideA) {
		throw std::invalid_argument("Zadane stranice ne cine trokut");
	}
}

} // namespace

PlaneShapeResult PlaneShapes::square(double side) {
	requirePositive(side);
	return {4.0 * side, side * side};
}

PlaneShapeResult PlaneShapes::rectangle(double width, double height) {
	requirePositive(width);
	requirePositive(height);
	return {2.0 * (width + height), width * height};
}

PlaneShapeResult PlaneShapes::triangle(double sideA, double sideB, double sideC) {
	requireTriangle(sideA, sideB, sideC);
	const double semiperimeter = (sideA + sideB + sideC) / 2.0;
	const double area = std::sqrt(semiperimeter * (semiperimeter - sideA)
		* (semiperimeter - sideB) * (semiperimeter - sideC));
	return {sideA + sideB + sideC, area};
}

PlaneShapeResult PlaneShapes::parallelogram(double base, double side, double height) {
	requirePositive(base);
	requirePositive(side);
	requirePositive(height);
	return {2.0 * (base + side), base * height};
}

PlaneShapeResult PlaneShapes::trapezoid(double baseA, double baseB, double sideC, double sideD, double height) {
	requirePositive(baseA);
	requirePositive(baseB);
	requirePositive(sideC);
	requirePositive(sideD);
	requirePositive(height);
	return {baseA + baseB + sideC + sideD, (baseA + baseB) * height / 2.0};
}

PlaneShapeResult PlaneShapes::circle(double radius) {
	requirePositive(radius);
	return {2.0 * std::numbers::pi * radius, std::numbers::pi * radius * radius};
}

} // namespace aksiomat::geometry
