#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/LogicParser.hpp"

using aksiomat::LogicParser;

TEST(LogicParser, ParsesUnicodeOperators) {
	auto expr = LogicParser::parse("(p \u2227 \u00ACq) \u2192 r");
	EXPECT_EQ(expr->toString(), "((p \u2227 \u00ACq) \u2192 r)");
}

TEST(LogicParser, ParsesAsciiOperators) {
	auto expr = LogicParser::parse("(p & !q) -> r");
	EXPECT_EQ(expr->toString(), "((p \u2227 \u00ACq) \u2192 r)");
}

TEST(LogicParser, OperatorPrecedence) {
	// ¬ > ∧ > ∨ > → > ↔
	auto expr = LogicParser::parse("!p & q | r -> s <-> t");
	EXPECT_EQ(expr->toString(), "((((\u00ACp \u2227 q) \u2228 r) \u2192 s) \u2194 t)");
}

TEST(LogicParser, ImplicationIsRightAssociative) {
	auto expr = LogicParser::parse("p -> q -> r");
	EXPECT_EQ(expr->toString(), "(p \u2192 (q \u2192 r))");
}

TEST(LogicParser, Constants) {
	EXPECT_TRUE(LogicParser::parse("T")->evaluate({}));
	EXPECT_FALSE(LogicParser::parse("F")->evaluate({}));
	EXPECT_TRUE(LogicParser::parse("1")->evaluate({}));
	EXPECT_FALSE(LogicParser::parse("0")->evaluate({}));
}

TEST(LogicParser, EvaluatesParsedFormula) {
	auto expr = LogicParser::parse("(p -> q) <-> (!p | q)");
	for (bool p : {false, true}) {
		for (bool q : {false, true}) {
			EXPECT_TRUE(expr->evaluate({{"p", p}, {"q", q}}));
		}
	}
}

TEST(LogicParser, CollectVariables) {
	auto expr = LogicParser::parse("(p & q) -> (r | p)");
	auto vars = LogicParser::collectVariables(expr);
	EXPECT_EQ(vars, (std::set<std::string>{"p", "q", "r"}));
}

TEST(LogicParser, MultiCharacterVariableNames) {
	auto expr = LogicParser::parse("kisa -> mokro");
	EXPECT_FALSE(expr->evaluate({{"kisa", true}, {"mokro", false}}));
	EXPECT_TRUE(expr->evaluate({{"kisa", false}, {"mokro", false}}));
}

TEST(LogicParser, InvalidFormulaThrows) {
	EXPECT_THROW(LogicParser::parse(""), std::invalid_argument);
	EXPECT_THROW(LogicParser::parse("p &"), std::invalid_argument);
	EXPECT_THROW(LogicParser::parse("(p | q"), std::invalid_argument);
	EXPECT_THROW(LogicParser::parse("p q"), std::invalid_argument);
	EXPECT_THROW(LogicParser::parse("& p"), std::invalid_argument);
}
