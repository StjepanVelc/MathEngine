#pragma once

#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct HigherOrderDerivativeResult {
	std::string expression;
	int order;
	std::string derivativeExpression;
	double point;
	double derivativeValueAtPoint;
	std::vector<std::string> steps;
};

struct ChainRuleResult {
	std::string outerExpression;
	std::string innerExpression;
	double point;
	double innerValueAtPoint;
	double outerDerivativeAtInnerValue;
	double innerDerivativeAtPoint;
	double compositeDerivativeAtPoint;
	std::vector<std::string> steps;
};

class AdvancedDerivatives {
public:
	// Racuna n-tu derivaciju polinoma i njenu vrijednost u tocki.
	static HigherOrderDerivativeResult nthDerivative(const std::string& expression, int order, double point,
		const std::string& variable = "x");

	// Racuna derivaciju kompozicije (f o g)'(point) = f'(g(point)) * g'(point) po lancanom pravilu.
	static ChainRuleResult chainRule(const std::string& outerExpression, const std::string& innerExpression,
		double point, const std::string& variable = "x");
};

} // namespace aksiomat::mathematical_analysis
