#include "aksiomat/integral_calculus/IntegralApplications.hpp"

#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/DefiniteIntegral.hpp"

namespace aksiomat::integral_calculus {
namespace {

IntegralApplicationResult finish(const std::string& quantity, const std::string& expression, const std::string& integrand,
	double a, double b, double factor, std::vector<std::string> steps) {
	if (!(a < b)) throw std::invalid_argument("Potrebno je a < b.");
	const auto d = DefiniteIntegration::integrate(integrand, a, b);
	IntegralApplicationResult r;
	r.quantity = quantity;
	r.expression = expression;
	r.integrand = integrand;
	r.lower = a;
	r.upper = b;
	r.exact = d.symbolic;
	r.antiderivative = d.antiderivative;
	r.value = factor * d.value;
	r.steps = std::move(steps);
	r.steps.push_back("Integral podintegralne funkcije na [a, b] iznosi " + differential_calculus::formatValue(d.value) +
		" (" + d.method + ").");
	return r;
}

} // namespace

IntegralApplicationResult IntegralApplications::volumeOfRevolution(const std::string& expression, double a, double b) {
	const std::string integrand = "(" + expression + ")^2";
	std::vector<std::string> steps = {
		"Rotacijom grafa y = f(x) oko osi x nastaje tijelo; presjek okomit na os je krug polumjera f(x).",
		"Povrsina presjeka je pi*f(x)^2, pa je volumen V = pi * integral od a do b od f(x)^2 dx (metoda diskova).",
		"Podintegralna funkcija je " + integrand + "."};
	auto r = finish("volumen rotacije", expression, integrand, a, b, 3.14159265358979323846, std::move(steps));
	r.steps.push_back("V = pi * integral = " + differential_calculus::formatValue(r.value) + ".");
	return r;
}

IntegralApplicationResult IntegralApplications::arcLength(const std::string& expression, double a, double b) {
	const auto derivative = differential_calculus::Differentiation::differentiate(expression, 1);
	const std::string integrand = "sqrt(1+(" + derivative.derivative + ")^2)";
	std::vector<std::string> steps = {
		"Mali dio luka ima duljinu sqrt(dx^2 + dy^2) = sqrt(1 + f'(x)^2) dx.",
		"Derivacija je f'(x) = " + derivative.derivative + ".",
		"Duljina luka je L = integral od a do b od sqrt(1 + f'(x)^2) dx, a podintegralna funkcija je " + integrand + "."};
	auto r = finish("duljina luka", expression, integrand, a, b, 1.0, std::move(steps));
	r.steps.push_back("L = " + differential_calculus::formatValue(r.value) + ".");
	return r;
}

} // namespace aksiomat::integral_calculus
