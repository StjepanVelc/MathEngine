#include "aksiomat/geometry/Triangles.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace aksiomat::geometry {
namespace {

constexpr double epsilon = 1e-9;

void requirePositive(double value, const char* message) {
	if (!std::isfinite(value) || value <= 0.0) throw std::invalid_argument(message);
}

bool nearlyEqual(double lhs, double rhs) {
	return std::abs(lhs - rhs) <= epsilon * std::max({1.0, std::abs(lhs), std::abs(rhs)});
}

} // namespace

TriangleClassification Triangles::classify(double sideA, double sideB, double sideC) {
	requirePositive(sideA, "Stranica mora biti pozitivan konacan broj");
	requirePositive(sideB, "Stranica mora biti pozitivan konacan broj");
	requirePositive(sideC, "Stranica mora biti pozitivan konacan broj");
	std::array sides{sideA, sideB, sideC};
	std::sort(sides.begin(), sides.end());
	if (sides[0] + sides[1] <= sides[2]) throw std::invalid_argument("Zadane stranice ne cine trokut");

	std::string bySides = "raznostranican";
	if (nearlyEqual(sides[0], sides[2])) bySides = "jednakostranican";
	else if (nearlyEqual(sides[0], sides[1]) || nearlyEqual(sides[1], sides[2])) bySides = "jednakokracan";

	const double smallerSquares = sides[0] * sides[0] + sides[1] * sides[1];
	const double largestSquare = sides[2] * sides[2];
	std::string byAngles = "ostrokutan";
	if (nearlyEqual(smallerSquares, largestSquare)) byAngles = "pravokutan";
	else if (smallerSquares < largestSquare) byAngles = "tupokutan";
	return {bySides, byAngles};
}

double Triangles::thirdAngle(double angleA, double angleB) {
	requirePositive(angleA, "Kut mora biti pozitivan konacan broj");
	requirePositive(angleB, "Kut mora biti pozitivan konacan broj");
	if (angleA >= 180.0 || angleB >= 180.0 || angleA + angleB >= 180.0) {
		throw std::invalid_argument("Kutovi trokuta moraju imati zbroj 180 stupnjeva");
	}
	return 180.0 - angleA - angleB;
}

double Triangles::hypotenuse(double legA, double legB) {
	requirePositive(legA, "Kateta mora biti pozitivan konacan broj");
	requirePositive(legB, "Kateta mora biti pozitivan konacan broj");
	return std::hypot(legA, legB);
}

double Triangles::leg(double hypotenuseValue, double knownLeg) {
	requirePositive(hypotenuseValue, "Hipotenuza mora biti pozitivan konacan broj");
	requirePositive(knownLeg, "Kateta mora biti pozitivan konacan broj");
	if (knownLeg >= hypotenuseValue) throw std::invalid_argument("Hipotenuza mora biti dulja od katete");
	return std::sqrt(hypotenuseValue * hypotenuseValue - knownLeg * knownLeg);
}

} // namespace aksiomat::geometry
