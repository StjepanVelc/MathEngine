#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/mathematical_analysis/FormalLimits.hpp"

using aksiomat::mathematical_analysis::FormalLimits;

TEST(FormalLimits, EvaluatesPolynomialLimitWithEpsilonDeltaTable) {
	const auto result = FormalLimits::evaluate("x^2 + 1", 2.0);
	EXPECT_DOUBLE_EQ(result.limitValue, 5.0);
	EXPECT_TRUE(result.existsFinite);
	EXPECT_EQ(result.epsilonDeltaTable.size(), 4u);
	for (const auto& sample : result.epsilonDeltaTable) {
		EXPECT_GT(sample.delta, 0.0);
	}
	EXPECT_FALSE(result.steps.empty());
}

TEST(FormalLimits, SmallerEpsilonYieldsSmallerOrEqualDelta) {
	const auto result = FormalLimits::evaluate("2x - 3", 1.0);
	for (std::size_t i = 1; i < result.epsilonDeltaTable.size(); ++i) {
		EXPECT_LE(result.epsilonDeltaTable[i].delta, result.epsilonDeltaTable[i - 1].delta + 1e-9);
	}
}

TEST(FormalLimits, RejectsNonFinitePoint) {
	EXPECT_THROW(FormalLimits::evaluate("x", std::numeric_limits<double>::infinity()), std::invalid_argument);
}

TEST(FormalLimits, ContinuousPolynomialPassesAllConditions) {
	const auto result = FormalLimits::checkContinuity("x^2 - 4", 2.0);
	EXPECT_TRUE(result.functionDefinedAtPoint);
	EXPECT_TRUE(result.isContinuous);
	EXPECT_DOUBLE_EQ(result.functionValueAtPoint, 0.0);
	EXPECT_EQ(result.steps.size(), 4u);
}
