#include "aksiomat/calculus_basics/Derivatives.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::calculus_basics {
namespace {

constexpr double epsilon = 1e-12;

std::string number(double value) {
	if (std::abs(value) < 1e-9) value = 0.0;
	std::ostringstream output;
	output.precision(10);
	output << value;
	return output.str();
}

} // namespace

DerivativeResult Derivatives::differentiate(const std::string& expression, double point, const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka deriviranja mora biti konacan broj");
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);
	const auto derivativePolynomial = polynomial.derivative();

	DerivativeResult result;
	result.original = polynomial.toString(variable);
	result.derivative = derivativePolynomial.toString(variable);
	result.pointValue = polynomial.evaluate(point);
	result.slopeAtPoint = derivativePolynomial.evaluate(point);

	const double intercept = result.pointValue - result.slopeAtPoint * point;
	std::ostringstream tangent;
	tangent << "y = " << number(result.slopeAtPoint) << variable;
	if (intercept >= 0.0) tangent << " + " << number(intercept);
	else tangent << " - " << number(-intercept);
	result.tangentLine = tangent.str();

	result.steps.push_back("f(" + variable + ") = " + result.original + ", pa je f'(" + variable + ") = " + result.derivative +
		" prema pravilu potencije i zbroja.");
	result.steps.push_back("f(" + std::to_string(point) + ") = " + std::to_string(result.pointValue) + " i f'(" +
		std::to_string(point) + ") = " + std::to_string(result.slopeAtPoint) + " (nagib tangente).");
	result.steps.push_back("Jednadzba tangente u toj tocki: " + result.tangentLine + ".");

	return result;
}

RateOfChangeResult Derivatives::rateOfChange(const std::string& expression, double a, double b, const std::string& variable) {
	if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument("Rubne vrijednosti moraju biti konacne");
	if (std::abs(b - a) < epsilon) throw std::invalid_argument("Tocke a i b moraju biti razlicite");
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);
	const auto derivativePolynomial = polynomial.derivative();

	RateOfChangeResult result;
	result.averageRate = (polynomial.evaluate(b) - polynomial.evaluate(a)) / (b - a);
	result.instantaneousRate = derivativePolynomial.evaluate(a);

	result.steps.push_back("Prosjecna brzina promjene na [" + std::to_string(a) + ", " + std::to_string(b) +
		"] je (f(b) - f(a)) / (b - a) = " + std::to_string(result.averageRate) + ".");
	result.steps.push_back("Trenutna brzina promjene u tocki a jednaka je f'(a) = " + std::to_string(result.instantaneousRate) + ".");

	return result;
}

} // namespace aksiomat::calculus_basics
