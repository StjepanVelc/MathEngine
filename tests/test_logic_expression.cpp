#include <gtest/gtest.h>

#include "aksiomat/LogicExpression.hpp"

using aksiomat::LogicExpression;

TEST(LogicExpression, ConstantEvaluation) {
	EXPECT_TRUE(LogicExpression::constant(true)->evaluate({}));
	EXPECT_FALSE(LogicExpression::constant(false)->evaluate({}));
}

TEST(LogicExpression, VariableEvaluation) {
	auto p = LogicExpression::variable("p");
	EXPECT_TRUE(p->evaluate({{"p", true}}));
	EXPECT_FALSE(p->evaluate({{"p", false}}));
	EXPECT_THROW(p->evaluate({}), std::out_of_range);
}

TEST(LogicExpression, NegationEvaluation) {
	auto expr = LogicExpression::negation(LogicExpression::variable("p"));
	EXPECT_FALSE(expr->evaluate({{"p", true}}));
	EXPECT_TRUE(expr->evaluate({{"p", false}}));
}

TEST(LogicExpression, ConjunctionTruthTable) {
	auto expr = LogicExpression::conjunction(
		LogicExpression::variable("p"), LogicExpression::variable("q"));
	EXPECT_TRUE(expr->evaluate({{"p", true}, {"q", true}}));
	EXPECT_FALSE(expr->evaluate({{"p", true}, {"q", false}}));
	EXPECT_FALSE(expr->evaluate({{"p", false}, {"q", true}}));
	EXPECT_FALSE(expr->evaluate({{"p", false}, {"q", false}}));
}

TEST(LogicExpression, ImplicationTruthTable) {
	auto expr = LogicExpression::implication(
		LogicExpression::variable("p"), LogicExpression::variable("q"));
	EXPECT_TRUE(expr->evaluate({{"p", false}, {"q", false}}));
	EXPECT_TRUE(expr->evaluate({{"p", false}, {"q", true}}));
	EXPECT_FALSE(expr->evaluate({{"p", true}, {"q", false}}));
	EXPECT_TRUE(expr->evaluate({{"p", true}, {"q", true}}));
}

TEST(LogicExpression, EquivalenceIsTautologyForImplicationRewrite) {
	// (p -> q) <-> (!p || q) mora biti tautologija.
	auto p = LogicExpression::variable("p");
	auto q = LogicExpression::variable("q");
	auto expr = LogicExpression::equivalence(
		LogicExpression::implication(p, q),
		LogicExpression::disjunction(LogicExpression::negation(p), q));
	for (bool pv : {false, true}) {
		for (bool qv : {false, true}) {
			EXPECT_TRUE(expr->evaluate({{"p", pv}, {"q", qv}}));
		}
	}
}

TEST(LogicExpression, ToString) {
	auto expr = LogicExpression::conjunction(
		LogicExpression::variable("p"),
		LogicExpression::negation(LogicExpression::variable("q")));
	EXPECT_EQ(expr->toString(), "(p \u2227 \u00ACq)");
}
