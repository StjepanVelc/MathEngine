#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/Arithmetic.hpp"

using aksiomat::Arithmetic;

TEST(Arithmetic, BasicOperations) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("2+3"), 5.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("7-10"), -3.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("4*2.5"), 10.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("9/2"), 4.5);
}

TEST(Arithmetic, OperatorPrecedence) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("2+3*4"), 14.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("10-6/2"), 7.0);
}

TEST(Arithmetic, Parentheses) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("(2+3)*4"), 20.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("2*(3+(4-1))"), 12.0);
}

TEST(Arithmetic, UnaryMinus) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("-5"), -5.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("-(2+3)"), -5.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("--4"), 4.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("3*-2"), -6.0);
}

TEST(Arithmetic, Whitespace) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("  2 +  3 * 4  "), 14.0);
}

TEST(Arithmetic, DecimalNumbers) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("0.5+0.25"), 0.75);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("1.5*2"), 3.0);
}

TEST(Arithmetic, DivisionByZeroThrows) {
	EXPECT_THROW(Arithmetic::evaluate("1/0"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("5/(3-3)"), std::invalid_argument);
}

TEST(Arithmetic, InvalidExpressionThrows) {
	EXPECT_THROW(Arithmetic::evaluate(""), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("2+"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("(2+3"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("abc"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("2 3"), std::invalid_argument);
}
