#include "aksiomat/calculus_basics/Limits.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::calculus_basics {
namespace {

constexpr double epsilon = 1e-9;

} // namespace

LimitResult Limits::evaluate(const std::string& expression, double point, const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka limesa mora biti konacan broj");
	const auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);

	LimitResult result;
	result.expression = polynomial.toString(variable);
	result.point = point;

	const std::vector<double> steps_h = {0.1, 0.01, 0.001, 0.0001};
	for (const double h : steps_h) {
		result.leftSamples.push_back({point - h, polynomial.evaluate(point - h)});
	}
	for (const double h : steps_h) {
		result.rightSamples.push_back({point + h, polynomial.evaluate(point + h)});
	}

	result.limitValue = polynomial.evaluate(point);
	result.existsFinite = std::isfinite(result.limitValue);

	std::ostringstream stepText;
	stepText << "Buduci da je f(x) = " << result.expression << " polinom, neprekidna je funkcija pa limes postoji svugdje.";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Kad x tezi " + std::to_string(point) + " s lijeva i s desna, vrijednosti funkcije priblizavaju se f(" +
		std::to_string(point) + ") = " + std::to_string(result.limitValue) + ".");
	result.steps.push_back("Zato je lim f(x) kad x->" + std::to_string(point) + " jednak f(" + std::to_string(point) + ").");

	return result;
}

} // namespace aksiomat::calculus_basics
