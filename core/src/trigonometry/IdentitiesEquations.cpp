#include "aksiomat/trigonometry/IdentitiesEquations.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "aksiomat/trigonometry/Angles.hpp"

namespace aksiomat::trigonometry {
namespace {

constexpr double tolerance = 1e-10;

void requireIdentity(TrigIdentity identity) {
	if (identity != TrigIdentity::Pythagorean && identity != TrigIdentity::TangentRatio &&
		identity != TrigIdentity::DoubleAngleSine && identity != TrigIdentity::DoubleAngleCosine) {
		throw std::invalid_argument("Nepoznat trigonometrijski identitet");
	}
}

void requireFunction(TrigFunction function) {
	if (function != TrigFunction::Sine && function != TrigFunction::Cosine && function != TrigFunction::Tangent) {
		throw std::invalid_argument("Nepoznata trigonometrijska funkcija");
	}
}

void addPeriodic(std::vector<double>& solutions, double base, double period, double minimum, double maximum) {
	const auto first = static_cast<long long>(std::ceil((minimum - base) / period - tolerance));
	const auto last = static_cast<long long>(std::floor((maximum - base) / period + tolerance));
	for (long long k = first; k <= last; ++k) {
		const double candidate = base + static_cast<double>(k) * period;
		if (candidate >= minimum - tolerance && candidate <= maximum + tolerance) solutions.push_back(candidate);
	}
}

} // namespace

IdentityCheck IdentitiesEquations::check(TrigIdentity identity, double angleDegrees) {
	requireIdentity(identity);
	if (!std::isfinite(angleDegrees)) throw std::invalid_argument("Kut mora biti konacan broj");
	const double radians = Angles::toRadians(angleDegrees);
	const double sine = std::sin(radians);
	const double cosine = std::cos(radians);
	double left = 0.0;
	double right = 0.0;
	std::string formula;
	if (identity == TrigIdentity::Pythagorean) {
		left = sine * sine + cosine * cosine;
		right = 1.0;
		formula = "sin^2(x) + cos^2(x) = 1";
	} else if (identity == TrigIdentity::TangentRatio) {
		if (std::abs(cosine) < tolerance) throw std::invalid_argument("Identitet tan(x)=sin(x)/cos(x) nije definiran kada je cos(x)=0");
		left = std::tan(radians);
		right = sine / cosine;
		formula = "tan(x) = sin(x) / cos(x)";
	} else if (identity == TrigIdentity::DoubleAngleSine) {
		left = std::sin(2.0 * radians);
		right = 2.0 * sine * cosine;
		formula = "sin(2x) = 2sin(x)cos(x)";
	} else {
		left = std::cos(2.0 * radians);
		right = cosine * cosine - sine * sine;
		formula = "cos(2x) = cos^2(x) - sin^2(x)";
	}
	return {std::abs(left - right) <= tolerance, left, right, formula};
}

std::vector<double> IdentitiesEquations::solve(TrigFunction function, double value, double minDegrees, double maxDegrees) {
	requireFunction(function);
	if (!std::isfinite(value) || !std::isfinite(minDegrees) || !std::isfinite(maxDegrees)) {
		throw std::invalid_argument("Vrijednost i granice intervala moraju biti konacni brojevi");
	}
	if (minDegrees > maxDegrees) throw std::invalid_argument("Donja granica intervala mora biti manja ili jednaka gornjoj");
	if ((function == TrigFunction::Sine || function == TrigFunction::Cosine) && (value < -1.0 || value > 1.0)) return {};

	std::vector<double> solutions;
	if (function == TrigFunction::Tangent) {
		const double base = Angles::toDegrees(std::atan(value));
		addPeriodic(solutions, base, 180.0, minDegrees, maxDegrees);
	} else if (function == TrigFunction::Sine) {
		const double principal = Angles::toDegrees(std::asin(value));
		addPeriodic(solutions, principal, 360.0, minDegrees, maxDegrees);
		addPeriodic(solutions, 180.0 - principal, 360.0, minDegrees, maxDegrees);
	} else {
		const double principal = Angles::toDegrees(std::acos(value));
		addPeriodic(solutions, principal, 360.0, minDegrees, maxDegrees);
		addPeriodic(solutions, -principal, 360.0, minDegrees, maxDegrees);
	}

	std::sort(solutions.begin(), solutions.end());
	solutions.erase(std::unique(solutions.begin(), solutions.end(), [](double lhs, double rhs) {
		return std::abs(lhs - rhs) <= tolerance;
	}), solutions.end());
	for (double& solution : solutions) {
		if (std::abs(solution) < tolerance) solution = 0.0;
	}
	return solutions;
}

} // namespace aksiomat::trigonometry
