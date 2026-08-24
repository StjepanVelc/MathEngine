#include "aksiomat/trigonometry/GeneralTriangle.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

#include "aksiomat/trigonometry/Angles.hpp"

namespace aksiomat::trigonometry {
namespace {

void requirePositive(double value, const char* name) {
	if (!std::isfinite(value) || value <= 0.0) throw std::invalid_argument(std::string(name) + " mora biti pozitivan konacan broj");
}

void requireAngle(double angle, const char* name) {
	if (!std::isfinite(angle) || angle <= 0.0 || angle >= 180.0) {
		throw std::invalid_argument(std::string(name) + " mora biti izmedu 0 i 180 stupnjeva");
	}
}

void requireTriangle(double a, double b, double c) {
	requirePositive(a, "Stranica a");
	requirePositive(b, "Stranica b");
	requirePositive(c, "Stranica c");
	if (a + b <= c || a + c <= b || b + c <= a) throw std::invalid_argument("Zadane stranice ne cine trokut");
}

double angleFromSides(double opposite, double first, double second) {
	double cosine = (first * first + second * second - opposite * opposite) / (2.0 * first * second);
	cosine = std::clamp(cosine, -1.0, 1.0);
	return Angles::toDegrees(std::acos(cosine));
}

double heron(double a, double b, double c) {
	const double s = (a + b + c) / 2.0;
	return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

} // namespace

GeneralTriangleSolution GeneralTriangle::fromSides(double sideA, double sideB, double sideC) {
	requireTriangle(sideA, sideB, sideC);
	const double angleA = angleFromSides(sideA, sideB, sideC);
	const double angleB = angleFromSides(sideB, sideA, sideC);
	const double angleC = 180.0 - angleA - angleB;
	return {sideA, sideB, sideC, angleA, angleB, angleC, heron(sideA, sideB, sideC), {
		"Poznate su sve tri stranice (SSS).",
		"Kutove racunamo zakonom kosinusa.",
		"Povrsinu racunamo Heronovom formulom."
	}};
}

GeneralTriangleSolution GeneralTriangle::fromTwoSidesAndIncludedAngle(double sideA, double sideB, double includedAngleC) {
	requirePositive(sideA, "Stranica a");
	requirePositive(sideB, "Stranica b");
	requireAngle(includedAngleC, "Kut gamma");
	const double radians = Angles::toRadians(includedAngleC);
	const double sideC = std::sqrt(sideA * sideA + sideB * sideB - 2.0 * sideA * sideB * std::cos(radians));
	requireTriangle(sideA, sideB, sideC);
	const double angleA = angleFromSides(sideA, sideB, sideC);
	const double angleB = 180.0 - angleA - includedAngleC;
	const double area = sideA * sideB * std::sin(radians) / 2.0;
	return {sideA, sideB, sideC, angleA, angleB, includedAngleC, area, {
		"Poznate su dvije stranice i kut izmedu njih (SAS).",
		"Trecu stranicu racunamo zakonom kosinusa.",
		"Povrsina: P = a*b*sin(gamma)/2.",
		"Preostale kutove odredujemo iz stranica i zbroja 180 stupnjeva."
	}};
}

GeneralTriangleSolution GeneralTriangle::fromSideAndTwoAngles(double sideA, double angleA, double angleB) {
	requirePositive(sideA, "Stranica a");
	requireAngle(angleA, "Kut alpha");
	requireAngle(angleB, "Kut beta");
	if (angleA + angleB >= 180.0) throw std::invalid_argument("Zbroj dvaju kutova mora biti manji od 180 stupnjeva");
	const double angleC = 180.0 - angleA - angleB;
	const double sineA = std::sin(Angles::toRadians(angleA));
	const double sideB = sideA * std::sin(Angles::toRadians(angleB)) / sineA;
	const double sideC = sideA * std::sin(Angles::toRadians(angleC)) / sineA;
	return {sideA, sideB, sideC, angleA, angleB, angleC, heron(sideA, sideB, sideC), {
		"Poznate su jedna stranica i dva kuta (ASA/AAS).",
		"Treci kut: gamma = 180 - alpha - beta.",
		"Preostale stranice racunamo zakonom sinusa.",
		"Povrsinu racunamo iz dobivenih stranica."
	}};
}

} // namespace aksiomat::trigonometry
