// CLI demo aplikacija — brzi način za isprobavanje jezgre bez browsera.
#include <iostream>

#include "aksiomat/LogicExpression.hpp"

int main() {
	using aksiomat::LogicExpression;

	// Demo: (p → q) ↔ (¬p ∨ q)
	auto p = LogicExpression::variable("p");
	auto q = LogicExpression::variable("q");
	auto expr = LogicExpression::equivalence(
		LogicExpression::implication(p, q),
		LogicExpression::disjunction(LogicExpression::negation(p), q));

	std::cout << "Izraz: " << expr->toString() << "\n\n";
	std::cout << "p q | rezultat\n";
	std::cout << "----+---------\n";
	for (bool pv : {false, true}) {
		for (bool qv : {false, true}) {
			bool result = expr->evaluate({{"p", pv}, {"q", qv}});
			std::cout << pv << " " << qv << " | " << (result ? "T" : "F") << "\n";
		}
	}
	return 0;
}
