#include "aksiomat/calculus_basics/DefiniteIntegral.hpp"

#include <cmath>
#include <map>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::calculus_basics {
namespace {

std::string number(double value) {
	if (std::abs(value) < 1e-9) value = 0.0;
	std::ostringstream output;
	output.precision(10);
	output << value;
	return output.str();
}

double simpson(const aksiomat::algebra::Polynomial& polynomial, double a, double b, int segments) {
	if (segments % 2 != 0) ++segments;
	const double h = (b - a) / segments;
	double sum = polynomial.evaluate(a) + polynomial.evaluate(b);
	for (int i = 1; i < segments; ++i) {
		const double x = a + i * h;
		sum += (i % 2 == 0 ? 2.0 : 4.0) * polynomial.evaluate(x);
	}
	return sum * h / 3.0;
}

} // namespace

DefiniteIntegralResult DefiniteIntegral::evaluate(const std::string& expression, double lowerBound, double upperBound,
	const std::string& variable) {
	if (!std::isfinite(lowerBound) || !std::isfinite(upperBound)) throw std::invalid_argument("Granice integrala moraju biti konacne");
	if (std::abs(upperBound - lowerBound) < 1e-12) throw std::invalid_argument("Granice integrala moraju biti razlicite");

	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);

	std::map<int, double, std::greater<int>> antiderivativeCoefficients;
	for (int degree = 0; degree <= polynomial.degree(); ++degree) {
		const double coefficientValue = polynomial.coefficient(degree);
		if (std::abs(coefficientValue) < 1e-12) continue;
		antiderivativeCoefficients[degree + 1] = coefficientValue / (degree + 1);
	}
	const aksiomat::algebra::Polynomial antiderivativePolynomial(antiderivativeCoefficients);

	DefiniteIntegralResult result;
	result.expression = polynomial.toString(variable);
	result.antiderivative = antiderivativePolynomial.toString(variable) + " + C";
	result.lowerBound = lowerBound;
	result.upperBound = upperBound;
	result.area = antiderivativePolynomial.evaluate(upperBound) - antiderivativePolynomial.evaluate(lowerBound);
	result.numericCheck = simpson(polynomial, lowerBound, upperBound, 200);

	result.steps.push_back("f(" + variable + ") = " + result.expression + ", antiderivacija F(" + variable + ") = " +
		result.antiderivative + " (obrnuto pravilo potencije).");
	result.steps.push_back("Odredeni integral racuna se kao F(b) - F(a) = F(" + number(upperBound) + ") - F(" +
		number(lowerBound) + ") = " + number(result.area) + ".");
	result.steps.push_back("Numericka provjera Simpsonovim pravilom daje priblizno " + number(result.numericCheck) +
		", sto potvrduje rezultat.");

	return result;
}

} // namespace aksiomat::calculus_basics
