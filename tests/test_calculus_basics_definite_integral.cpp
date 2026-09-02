#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/calculus_basics/DefiniteIntegral.hpp"

using aksiomat::calculus_basics::DefiniteIntegral;

TEST(DefiniteIntegral, ComputesAreaUnderPolynomial) {
	const auto result = DefiniteIntegral::evaluate("x^2", 0.0, 3.0);
	EXPECT_DOUBLE_EQ(result.area, 9.0);
	EXPECT_NEAR(result.numericCheck, 9.0, 1e-6);
	EXPECT_EQ(result.antiderivative, "0.333333333333x^3 + C");
}

TEST(DefiniteIntegral, ComputesAreaForLinearFunction) {
	const auto result = DefiniteIntegral::evaluate("2x + 1", 0.0, 2.0);
	EXPECT_DOUBLE_EQ(result.area, 6.0);
	EXPECT_NEAR(result.numericCheck, 6.0, 1e-6);
}

TEST(DefiniteIntegral, ValidatesBounds) {
	EXPECT_THROW(DefiniteIntegral::evaluate("x", 1.0, 1.0), std::invalid_argument);
}
