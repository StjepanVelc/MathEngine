#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

using aksiomat::algebra::Polynomial;

TEST(Polynomial, ParsesAndNormalizesExpressions) {
	const auto polynomial = Polynomial::parse("(x+1)(x-1) + 2x");
	EXPECT_EQ(polynomial.toString(), "x^2 + 2x - 1");
	EXPECT_EQ(polynomial.degree(), 2);
}

TEST(Polynomial, AnalyzesQuadraticShapeAndFactorization) {
	const auto polynomial = Polynomial::parse("x^2 - 5x + 6");
	ASSERT_TRUE(polynomial.discriminant().has_value());
	EXPECT_DOUBLE_EQ(*polynomial.discriminant(), 1.0);
	ASSERT_TRUE(polynomial.vertex().has_value());
	EXPECT_DOUBLE_EQ(polynomial.vertex()->x, 2.5);
	EXPECT_DOUBLE_EQ(polynomial.vertex()->y, -0.25);
	ASSERT_TRUE(polynomial.factorizedForm().has_value());
	EXPECT_EQ(*polynomial.factorizedForm(), "(x - 2)(x - 3)");

	EXPECT_EQ(*Polynomial::parse("2x^2 - 8x + 8").factorizedForm(), "2(x - 2)(x - 2)");
	EXPECT_FALSE(Polynomial::parse("x^2 + 1").factorizedForm().has_value());
}

TEST(Polynomial, PerformsBasicOperations) {
	const auto first = Polynomial::parse("x + 1");
	const auto second = Polynomial::parse("x - 1");
	EXPECT_EQ((first + second).toString(), "2x");
	EXPECT_EQ((first - second).toString(), "2");
	EXPECT_EQ((first * second).toString(), "x^2 - 1");
}

TEST(Polynomial, EvaluatesAndDifferentiates) {
	const auto polynomial = Polynomial::parse("3x^2 - 2x + 4");
	EXPECT_DOUBLE_EQ(polynomial.evaluate(2.0), 12.0);
	EXPECT_EQ(polynomial.derivative().toString(), "6x - 2");
}

TEST(Polynomial, FindsLinearAndQuadraticRealRoots) {
	EXPECT_EQ(Polynomial::parse("2x - 4").realRoots(), std::vector<double>({2.0}));
	EXPECT_EQ(Polynomial::parse("x^2 - 5x + 6").realRoots(), std::vector<double>({2.0, 3.0}));
	EXPECT_TRUE(Polynomial::parse("x^2 + 1").realRoots().empty());
}

TEST(Polynomial, RejectsUnsupportedForms) {
	EXPECT_THROW(Polynomial::parse("1/x"), std::invalid_argument);
	EXPECT_THROW(Polynomial::parse("x^-1"), std::invalid_argument);
	EXPECT_THROW(Polynomial::parse("x^3").realRoots(), std::invalid_argument);
}
