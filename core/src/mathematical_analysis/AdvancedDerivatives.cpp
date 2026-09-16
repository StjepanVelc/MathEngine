#include "aksiomat/mathematical_analysis/AdvancedDerivatives.hpp"

#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::mathematical_analysis {

namespace {
constexpr int maximumDerivativeOrder = 50;
} // namespace

HigherOrderDerivativeResult AdvancedDerivatives::nthDerivative(const std::string& expression, int order, double point,
	const std::string& variable) {
	if (order < 1) throw std::invalid_argument("Red derivacije mora biti barem 1");
	if (order > maximumDerivativeOrder) throw std::invalid_argument("Red derivacije ne smije biti veci od " + std::to_string(maximumDerivativeOrder));

	auto polynomial = aksiomat::algebra::Polynomial::parse(expression, variable);

	HigherOrderDerivativeResult result;
	result.expression = polynomial.toString(variable);
	result.order = order;

	for (int i = 0; i < order; ++i) {
		polynomial = polynomial.derivative();
	}

	result.derivativeExpression = polynomial.toString(variable);
	result.point = point;
	result.derivativeValueAtPoint = polynomial.evaluate(point);

	std::ostringstream stepText;
	stepText << "Polazna funkcija f(" << variable << ") = " << result.expression << ".";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Deriviramo " + std::to_string(order) + " puta uzastopno, svaki put primjenjujuci pravila deriviranja polinoma.");
	result.steps.push_back("f^(" + std::to_string(order) + ")(" + variable + ") = " + result.derivativeExpression + ".");
	result.steps.push_back("Vrijednost u tocki " + variable + " = " + std::to_string(point) + " je f^(" +
		std::to_string(order) + ")(" + std::to_string(point) + ") = " + std::to_string(result.derivativeValueAtPoint) + ".");

	return result;
}

ChainRuleResult AdvancedDerivatives::chainRule(const std::string& outerExpression, const std::string& innerExpression,
	double point, const std::string& variable) {
	const auto inner = aksiomat::algebra::Polynomial::parse(innerExpression, variable);
	const auto outer = aksiomat::algebra::Polynomial::parse(outerExpression, variable);

	ChainRuleResult result;
	result.outerExpression = outer.toString(variable);
	result.innerExpression = inner.toString(variable);
	result.point = point;

	result.innerValueAtPoint = inner.evaluate(point);
	const auto outerDerivative = outer.derivative();
	result.outerDerivativeAtInnerValue = outerDerivative.evaluate(result.innerValueAtPoint);
	const auto innerDerivative = inner.derivative();
	result.innerDerivativeAtPoint = innerDerivative.evaluate(point);
	result.compositeDerivativeAtPoint = result.outerDerivativeAtInnerValue * result.innerDerivativeAtPoint;

	result.steps.push_back("Neka je h(" + variable + ") = f(g(" + variable + ")), gdje je f(" + variable + ") = " +
		result.outerExpression + " i g(" + variable + ") = " + result.innerExpression + ".");
	result.steps.push_back("Lancano pravilo: h'(" + variable + ") = f'(g(" + variable + ")) * g'(" + variable + ").");
	result.steps.push_back("g(" + std::to_string(point) + ") = " + std::to_string(result.innerValueAtPoint) + ".");
	result.steps.push_back("f'(g(" + std::to_string(point) + ")) = " + std::to_string(result.outerDerivativeAtInnerValue) +
		", g'(" + std::to_string(point) + ") = " + std::to_string(result.innerDerivativeAtPoint) + ".");
	result.steps.push_back("h'(" + std::to_string(point) + ") = " + std::to_string(result.compositeDerivativeAtPoint) + ".");

	return result;
}

} // namespace aksiomat::mathematical_analysis
