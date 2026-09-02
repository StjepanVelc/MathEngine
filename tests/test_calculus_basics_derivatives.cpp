#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/calculus_basics/Derivatives.hpp"

using aksiomat::calculus_basics::Derivatives;

TEST(Derivatives, DifferentiatesPolynomialAndBuildsTangent) {
	const auto result = Derivatives::differentiate("x^2 - 4x + 3", 3.0);
	EXPECT_EQ(result.derivative, "2x - 4");
	EXPECT_DOUBLE_EQ(result.pointValue, 0.0);
	EXPECT_DOUBLE_EQ(result.slopeAtPoint, 2.0);
	EXPECT_EQ(result.tangentLine, "y = 2x - 6");
}

TEST(Derivatives, ComputesRateOfChange) {
	const auto result = Derivatives::rateOfChange("x^2", 1.0, 3.0);
	EXPECT_DOUBLE_EQ(result.averageRate, 4.0);
	EXPECT_DOUBLE_EQ(result.instantaneousRate, 2.0);
}

TEST(Derivatives, ValidatesInputs) {
	EXPECT_THROW(Derivatives::differentiate("x", std::numeric_limits<double>::infinity()), std::invalid_argument);
	EXPECT_THROW(Derivatives::rateOfChange("x", 1.0, 1.0), std::invalid_argument);
}
