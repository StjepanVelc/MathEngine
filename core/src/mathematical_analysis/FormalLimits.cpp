#include "aksiomat/mathematical_analysis/FormalLimits.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::mathematical_analysis {
namespace {

constexpr double kEpsilons[] = {1.0, 0.1, 0.01, 0.001};

// Za dani epsilon, pronadi (grubo) najveci delta takav da |x - point| < delta povlaci |f(x) - L| < epsilon,
// binarnom pretragom nad delta u razumnom rasponu.
double findDelta(const aksiomat::algebra::Polynomial& polynomial, double point, double limitValue, double eps) {
	double lo = 0.0;
	double hi = 1.0;
	constexpr int iterations = 40;
	for (int i = 0; i < iterations; ++i) {
		const double mid = (lo + hi) / 2.0;
		const double left = std::abs(polynomial.evaluate(point - mid) - limitValue);
		const double right = std::abs(polynomial.evaluate(point + mid) - limitValue);
		if (left < eps && right < eps) {
			lo = mid;
		} else {
			hi = mid;
		}
	}
	return lo;
}

} // namespace

FormalLimitResult FormalLimits::evaluate(const std::string& expression, double point, const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka limesa mora biti konacan broj");
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);

	FormalLimitResult result;
	result.expression = polynomial.toString(variable);
	result.point = point;
	result.limitValue = polynomial.evaluate(point);
	result.existsFinite = std::isfinite(result.limitValue);

	for (const double eps : kEpsilons) {
		const double delta = findDelta(polynomial, point, result.limitValue, eps);
		result.epsilonDeltaTable.push_back({eps, delta});
	}

	std::ostringstream stepText;
	stepText << "Tvrdnja: lim f(x) kad x->" << point << " jednak je L = " << result.limitValue << ".";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Formalna definicija: za svaki epsilon > 0 postoji delta > 0 takav da 0 < |x - " +
		std::to_string(point) + "| < delta povlaci |f(x) - L| < epsilon.");
	result.steps.push_back("Tablica pokazuje da manji epsilon zahtijeva manji delta, sto potvrdjuje da limes postoji formalno, ne samo intuitivno.");

	return result;
}

ContinuityResult FormalLimits::checkContinuity(const std::string& expression, double point, const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka mora biti konacan broj");
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);

	ContinuityResult result;
	result.expression = polynomial.toString(variable);
	result.point = point;
	result.functionDefinedAtPoint = true;
	result.functionValueAtPoint = polynomial.evaluate(point);
	result.limitValue = polynomial.evaluate(point);
	result.isContinuous = result.functionDefinedAtPoint &&
		std::isfinite(result.limitValue) &&
		std::abs(result.functionValueAtPoint - result.limitValue) < 1e-9;

	result.steps.push_back("Uvjet 1: f(" + std::to_string(point) + ") je definirana i iznosi " +
		std::to_string(result.functionValueAtPoint) + ".");
	result.steps.push_back("Uvjet 2: lim f(x) kad x->" + std::to_string(point) + " postoji i iznosi " +
		std::to_string(result.limitValue) + ".");
	result.steps.push_back("Uvjet 3: f(" + std::to_string(point) + ") = lim f(x), pa je funkcija neprekidna u toj tocki.");
	if (result.isContinuous) {
		result.steps.push_back("Zakljucak: funkcija je neprekidna u x = " + std::to_string(point) + ".");
	} else {
		result.steps.push_back("Zakljucak: barem jedan uvjet neprekidnosti nije zadovoljen.");
	}

	return result;
}

} // namespace aksiomat::mathematical_analysis
