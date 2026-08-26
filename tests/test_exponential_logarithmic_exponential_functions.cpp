#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/exponential_logarithmic/ExponentialFunctions.hpp"

using namespace aksiomat::exponential_logarithmic;

TEST(ExponentialFunctions, EvaluatesAndValidates) {
	const ExponentialFunction growth{2, 3};
	EXPECT_DOUBLE_EQ(ExponentialFunctions::evaluate(growth, 2), 18);
	EXPECT_THROW(ExponentialFunctions::evaluate({1, 0}, 1), std::invalid_argument);
	EXPECT_THROW(ExponentialFunctions::evaluate({1, 1}, 1), std::invalid_argument);
	EXPECT_THROW(ExponentialFunctions::evaluate({1, -1}, 1), std::invalid_argument);
}

TEST(ExponentialFunctions, AnalyzesGrowthAndDecay) {
	const auto growthAnalysis = ExponentialFunctions::analyze({1, 2}, 0, 4, 5);
	EXPECT_TRUE(growthAnalysis.isGrowth);
	EXPECT_DOUBLE_EQ(growthAnalysis.growthRatePercent, 100);
	EXPECT_EQ(growthAnalysis.samples.size(), 5u);

	const auto decayAnalysis = ExponentialFunctions::analyze({1, 0.5}, 0, 4, 5);
	EXPECT_FALSE(decayAnalysis.isGrowth);
	EXPECT_THROW(ExponentialFunctions::analyze({1, 2}, 4, 0, 5), std::invalid_argument);
}

TEST(ExponentialFunctions, ComputesGrowthDecayAndTimescales) {
	EXPECT_NEAR(ExponentialFunctions::growthDecayValue(100, 10, 1), 110, 1e-9);
	EXPECT_NEAR(ExponentialFunctions::halfLife(std::log(2.0)), 1, 1e-9);
	EXPECT_NEAR(ExponentialFunctions::doublingTime(100), 1, 1e-9);
	EXPECT_THROW(ExponentialFunctions::halfLife(-1), std::invalid_argument);
	EXPECT_THROW(ExponentialFunctions::doublingTime(0), std::invalid_argument);
}
