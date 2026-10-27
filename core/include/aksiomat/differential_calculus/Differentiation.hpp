#pragma once

#include <string>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::differential_calculus {

struct DerivativeResult {
	std::string expression;
	int order = 1;
	std::string rawDerivative;
	std::string derivative;
	std::vector<std::string> steps;
};

class Differentiation {
public:
	// Simbolicka derivacija; ako je steps zadan, u njega se upisuje primijenjeno pravilo za svaki cvor.
	static FnExpr::Ptr derive(const FnExpr::Ptr& expression, std::vector<std::string>* steps = nullptr);

	// Derivacija reda 1..10 uz objasnjenje po koracima (pravila: zbroj, produkt, kvocijent, potencija, lancano).
	static DerivativeResult differentiate(const std::string& expression, int order = 1,
		const std::string& variable = "x");

	// Izvrednjava (pojednostavljenu) derivaciju zadanog reda u tocki; NaN ako nije definirana.
	static double derivativeAt(const std::string& expression, double point, int order = 1,
		const std::string& variable = "x");
};

// Jesu li dva izraza numericki jednaka na zajednickoj domeni (za provjeru odgovora u vjezbama).
bool equivalentFunctions(const std::string& first, const std::string& second, const std::string& variable = "x");

} // namespace aksiomat::differential_calculus
