#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/calculus_basics/Limits.hpp"

using aksiomat::calculus_basics::Limits;

TEST(Limits, EvaluatesPolynomialLimitAtPoint) {
	const auto result = Limits::evaluate("x^2 + 1", 2.0);
	EXPECT_DOUBLE_EQ(result.limitValue, 5.0);
	EXPECT_TRUE(result.existsFinite);
	EXPECT_EQ(result.leftSamples.size(), 4u);
	EXPECT_EQ(result.rightSamples.size(), 4u);
	EXPECT_FALSE(result.steps.empty());
}

TEST(Limits, LeftAndRightSamplesConvergeToLimit) {
	const auto result = Limits::evaluate("2x - 3", 1.0);
	EXPECT_DOUBLE_EQ(result.limitValue, -1.0);
	for (const auto& sample : result.leftSamples) EXPECT_NEAR(sample.value, -1.0, 0.3);
	for (const auto& sample : result.rightSamples) EXPECT_NEAR(sample.value, -1.0, 0.3);
}

TEST(Limits, RejectsNonFinitePoint) {
	EXPECT_THROW(Limits::evaluate("x", std::numeric_limits<double>::infinity()), std::invalid_argument);
}
