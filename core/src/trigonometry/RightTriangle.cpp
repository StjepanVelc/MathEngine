#include "aksiomat/trigonometry/RightTriangle.hpp"

#include <cmath>
#include <stdexcept>

#include "aksiomat/trigonometry/Angles.hpp"

namespace aksiomat::trigonometry {
namespace {

void requirePositive(double value, const char* name) {
	if (!std::isfinite(value) || value <= 0.0) throw std::invalid_argument(std::string(name) + " mora biti pozitivan konacan broj");
}

void requireAcute(double angle) {
	if (!std::isfinite(angle) || angle <= 0.0 || angle >= 90.0) {
		throw std::invalid_argument("Ostri kut pravokutnog trokuta mora biti izmedu 0 i 90 stupnjeva");
	}
}

void requireSideType(KnownSide type) {
	if (type != KnownSide::Opposite && type != KnownSide::Adjacent && type != KnownSide::Hypotenuse) {
		throw std::invalid_argument("Nepoznata vrsta stranice");
	}
}

RightTriangleSolution build(double opposite, double adjacent, std::vector<std::string> steps) {
	const double hypotenuse = std::hypot(opposite, adjacent);
	const double angle = Angles::toDegrees(std::atan2(opposite, adjacent));
	return {opposite, adjacent, hypotenuse, angle, 90.0 - angle, std::move(steps)};
}

} // namespace

RightTriangleSolution RightTriangle::fromLegs(double opposite, double adjacent) {
	requirePositive(opposite, "Nasuprotna kateta");
	requirePositive(adjacent, "Prilezeca kateta");
	return build(opposite, adjacent, {
		"Poznate su obje katete.",
		"Hipotenuza: c = sqrt(a^2 + b^2).",
		"Kut: alpha = arctan(a / b).",
		"Drugi ostri kut: beta = 90 - alpha."
	});
}

RightTriangleSolution RightTriangle::fromLegAndHypotenuse(double leg, double hypotenuse, KnownSide legType) {
	requirePositive(leg, "Kateta");
	requirePositive(hypotenuse, "Hipotenuza");
	requireSideType(legType);
	if (legType == KnownSide::Hypotenuse) throw std::invalid_argument("Odaberi je li poznata nasuprotna ili prilezeca kateta");
	if (leg >= hypotenuse) throw std::invalid_argument("Hipotenuza mora biti dulja od katete");
	const double otherLeg = std::sqrt(hypotenuse * hypotenuse - leg * leg);
	const double opposite = legType == KnownSide::Opposite ? leg : otherLeg;
	const double adjacent = legType == KnownSide::Adjacent ? leg : otherLeg;
	auto solution = build(opposite, adjacent, {
		"Poznate su jedna kateta i hipotenuza.",
		"Druga kateta: sqrt(c^2 - poznata_kateta^2).",
		"Kut dobivamo inverznom trigonometrijskom funkcijom."
	});
	solution.hypotenuse = hypotenuse;
	return solution;
}

RightTriangleSolution RightTriangle::fromSideAndAngle(double side, KnownSide sideType, double angleDegrees) {
	requirePositive(side, "Stranica");
	requireSideType(sideType);
	requireAcute(angleDegrees);
	const double radians = Angles::toRadians(angleDegrees);
	double opposite = 0.0;
	double adjacent = 0.0;
	if (sideType == KnownSide::Hypotenuse) {
		opposite = side * std::sin(radians);
		adjacent = side * std::cos(radians);
	} else if (sideType == KnownSide::Opposite) {
		opposite = side;
		adjacent = side / std::tan(radians);
	} else {
		adjacent = side;
		opposite = side * std::tan(radians);
	}
	auto solution = build(opposite, adjacent, {
		"Poznati su jedna stranica i ostri kut alpha.",
		"sin(alpha) = nasuprotna / hipotenuza.",
		"cos(alpha) = prilezeca / hipotenuza.",
		"tan(alpha) = nasuprotna / prilezeca."
	});
	solution.angleDegrees = angleDegrees;
	solution.complementaryAngleDegrees = 90.0 - angleDegrees;
	if (sideType == KnownSide::Hypotenuse) solution.hypotenuse = side;
	return solution;
}

} // namespace aksiomat::trigonometry
