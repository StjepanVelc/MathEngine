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

TEST(Arithmetic, PowerIsRightAssociativeAndPrecedesUnaryMinus) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("2^3^2"), 512.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("-2^2"), -4.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("(-2)^2"), 4.0);
}

TEST(Arithmetic, FactorialAndModulo) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("5!"), 120.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("3!^2"), 36.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("17%5"), 2.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("mod(17,5)"), 2.0);
}

TEST(Arithmetic, FunctionsAndConstants) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("sqrt(16)"), 4.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("abs(-5)"), 5.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("min(4,2)+max(3,7)"), 9.0);
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("round(2.6)+floor(2.9)+ceil(2.1)"), 8.0);
	EXPECT_NEAR(Arithmetic::evaluate("pi"), 3.141592653589793, 1e-14);
	EXPECT_NEAR(Arithmetic::evaluate("e"), 2.718281828459045, 1e-14);
}

TEST(Arithmetic, ScientificNotation) {
	EXPECT_DOUBLE_EQ(Arithmetic::evaluate("1.5e3 + 2.5E-1"), 1500.25);
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
	EXPECT_THROW(Arithmetic::evaluate("5%0"), std::invalid_argument);
}

TEST(Arithmetic, DomainErrorsThrow) {
	EXPECT_THROW(Arithmetic::evaluate("sqrt(-1)"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("(-1)!"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("2.5!"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("171!"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("unknown(1)"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("sqrt(1,2)"), std::invalid_argument);
}

TEST(Arithmetic, InvalidExpressionThrows) {
	EXPECT_THROW(Arithmetic::evaluate(""), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("2+"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("(2+3"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("abc"), std::invalid_argument);
	EXPECT_THROW(Arithmetic::evaluate("2 3"), std::invalid_argument);
}
