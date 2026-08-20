#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/AlgebraParser.hpp"

using aksiomat::algebra::AlgebraOp;
using aksiomat::algebra::AlgebraParser;

TEST(AlgebraParser, ParsesVariablesAndPrecedence) {
	const auto expression = AlgebraParser::parse("2 + 3*x");
	ASSERT_EQ(expression->op(), AlgebraOp::Add);
	ASSERT_EQ(expression->rhs()->op(), AlgebraOp::Multiply);
	EXPECT_EQ(expression->rhs()->rhs()->name(), "x");
}

TEST(AlgebraParser, ParsesImplicitMultiplication) {
	const auto coefficient = AlgebraParser::parse("2x");
	EXPECT_EQ(coefficient->op(), AlgebraOp::Multiply);

	const auto parenthesized = AlgebraParser::parse("2(x+1)");
	EXPECT_EQ(parenthesized->op(), AlgebraOp::Multiply);
	EXPECT_EQ(parenthesized->rhs()->op(), AlgebraOp::Add);

	const auto factors = AlgebraParser::parse("(x+1)(x-1)");
	EXPECT_EQ(factors->op(), AlgebraOp::Multiply);
	EXPECT_EQ(factors->lhs()->op(), AlgebraOp::Add);
	EXPECT_EQ(factors->rhs()->op(), AlgebraOp::Subtract);
}

TEST(AlgebraParser, PowerIsRightAssociativeAndPrecedesUnaryMinus) {
	const auto power = AlgebraParser::parse("x^2^3");
	ASSERT_EQ(power->op(), AlgebraOp::Power);
	EXPECT_EQ(power->rhs()->op(), AlgebraOp::Power);

	const auto negative = AlgebraParser::parse("-x^2");
	ASSERT_EQ(negative->op(), AlgebraOp::Negate);
	EXPECT_EQ(negative->lhs()->op(), AlgebraOp::Power);
}

TEST(AlgebraParser, SupportsIdentifiersAndScientificNumbers) {
	const auto expression = AlgebraParser::parse("1.5e2*velocity_1");
	ASSERT_EQ(expression->op(), AlgebraOp::Multiply);
	EXPECT_DOUBLE_EQ(expression->lhs()->value(), 150.0);
	EXPECT_EQ(expression->rhs()->name(), "velocity_1");
}

TEST(AlgebraParser, RejectsInvalidExpressions) {
	EXPECT_THROW(AlgebraParser::parse(""), std::invalid_argument);
	EXPECT_THROW(AlgebraParser::parse("x+"), std::invalid_argument);
	EXPECT_THROW(AlgebraParser::parse("(x+1"), std::invalid_argument);
	EXPECT_THROW(AlgebraParser::parse("x**2"), std::invalid_argument);
}
