#include "aksiomat/calculus_basics/DerivativeApplications.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::calculus_basics {
namespace {

constexpr double epsilon = 1e-9;

std::string number(double value) {
	if (std::abs(value) < 1e-9) value = 0.0;
	std::ostringstream output;
	output.precision(10);
	output << value;
	return output.str();
}

double sampleBetween(const aksiomat::algebra::Polynomial& derivative, double lower, double upper) {
	if (!std::isfinite(lower) && !std::isfinite(upper)) return derivative.evaluate(0.0);
	if (!std::isfinite(lower)) return derivative.evaluate(upper - 1.0);
	if (!std::isfinite(upper)) return derivative.evaluate(lower + 1.0);
	return derivative.evaluate((lower + upper) / 2.0);
}

std::string boundaryText(double value, bool isLower) {
	if (!std::isfinite(value)) return isLower ? "-inf" : "+inf";
	return number(value);
}

} // namespace

DerivativeApplicationResult DerivativeApplications::analyze(const std::string& expression, const std::string& variable) {
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);
	const auto derivativePolynomial = polynomial.derivative();
	if (derivativePolynomial.degree() > 2) {
		throw std::invalid_argument("Analiza ekstrema trenutno podrzava polinome ciji je izvod najvise drugog stupnja (izvorni polinom do 3. stupnja)");
	}
	const auto secondDerivative = derivativePolynomial.derivative();

	DerivativeApplicationResult result;
	result.expression = polynomial.toString(variable);
	result.derivative = derivativePolynomial.toString(variable);

	auto criticalXs = derivativePolynomial.realRoots();
	std::sort(criticalXs.begin(), criticalXs.end());

	for (const double x : criticalXs) {
		const double y = polynomial.evaluate(x);
		const double secondValue = secondDerivative.evaluate(x);
		std::string kind;
		if (secondValue > epsilon) kind = "minimum";
		else if (secondValue < -epsilon) kind = "maksimum";
		else kind = "prijevojna tocka";
		result.criticalPoints.push_back({x, y, kind});
	}

	std::vector<double> boundaries;
	boundaries.push_back(-std::numeric_limits<double>::infinity());
	for (const double x : criticalXs) boundaries.push_back(x);
	boundaries.push_back(std::numeric_limits<double>::infinity());

	for (std::size_t index = 0; index + 1 < boundaries.size(); ++index) {
		const double lower = boundaries[index];
		const double upper = boundaries[index + 1];
		const double slope = sampleBetween(derivativePolynomial, lower, upper);
		const std::string interval = "(" + boundaryText(lower, true) + ", " + boundaryText(upper, false) + ")";
		if (slope > epsilon) result.increasingIntervals.push_back(interval);
		else if (slope < -epsilon) result.decreasingIntervals.push_back(interval);
	}

	result.steps.push_back("f(" + variable + ") = " + result.expression + ", f'(" + variable + ") = " + result.derivative + ".");
	if (result.criticalPoints.empty()) {
		result.steps.push_back("f'(" + variable + ") nema realnih nultocaka, pa funkcija nema lokalnih ekstrema na cijelom podrucju definicije.");
	} else {
		for (const auto& point : result.criticalPoints) {
			result.steps.push_back("U tocki " + variable + " = " + number(point.x) + " funkcija ima " + point.kind + " f(" +
				number(point.x) + ") = " + number(point.y) + ".");
		}
	}
	result.steps.push_back("Predznak f'(" + variable + ") odreduje intervale rasta i pada funkcije.");

	return result;
}

} // namespace aksiomat::calculus_basics
