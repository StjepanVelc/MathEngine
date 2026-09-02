#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/calculus_basics/DerivativeApplications.hpp"

using aksiomat::calculus_basics::DerivativeApplications;

TEST(DerivativeApplications, FindsExtremaAndMonotonicity) {
	const auto result = DerivativeApplications::analyze("x^3 - 3x");
	ASSERT_EQ(result.criticalPoints.size(), 2u);
	EXPECT_DOUBLE_EQ(result.criticalPoints[0].x, -1.0);
	EXPECT_EQ(result.criticalPoints[0].kind, "maksimum");
	EXPECT_DOUBLE_EQ(result.criticalPoints[1].x, 1.0);
	EXPECT_EQ(result.criticalPoints[1].kind, "minimum");
	EXPECT_EQ(result.increasingIntervals.size(), 2u);
	EXPECT_EQ(result.decreasingIntervals.size(), 1u);
}

TEST(DerivativeApplications, HandlesNoExtrema) {
	const auto result = DerivativeApplications::analyze("2x + 5");
	EXPECT_TRUE(result.criticalPoints.empty());
	EXPECT_EQ(result.increasingIntervals.size(), 1u);
	EXPECT_TRUE(result.decreasingIntervals.empty());
}

TEST(DerivativeApplications, RejectsHighDegreeDerivatives) {
	EXPECT_THROW(DerivativeApplications::analyze("x^5"), std::invalid_argument);
}
