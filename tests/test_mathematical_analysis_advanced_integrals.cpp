#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/mathematical_analysis/AdvancedIntegrals.hpp"

using aksiomat::mathematical_analysis::AdvancedIntegrals;

TEST(AdvancedIntegrals, ConvergentImproperIntegralOfDecayingExponential) {
	// integral from 0 to infinity of e^(-x) dx = 1
	const auto result = AdvancedIntegrals::improperIntegral([](double x) { return std::exp(-x); }, "e^(-x)", 0.0);
	EXPECT_TRUE(result.convergent);
	EXPECT_NEAR(result.approximateValue, 1.0, 0.05);
	EXPECT_EQ(result.partialSums.size(), 4u);
	EXPECT_FALSE(result.steps.empty());
}

TEST(AdvancedIntegrals, RejectsNonFiniteLowerBound) {
	EXPECT_THROW(AdvancedIntegrals::improperIntegral([](double x) { return x; }, "x",
		std::numeric_limits<double>::infinity()), std::invalid_argument);
}

TEST(AdvancedIntegrals, IntegratesBySubstitutionForLinearPower) {
	// integral of (2x+1)^2 dx from 0 to 1
	// antiderivative: (2x+1)^3 / 6 => at 1: 27/6=4.5, at 0: 1/6 => diff = 4.5 - 1/6 = 4.333...
	const auto result = AdvancedIntegrals::integrateBySubstitution(2.0, 1.0, 2, 0.0, 1.0);
	EXPECT_NEAR(result.value, 4.333333333, 1e-6);
	EXPECT_FALSE(result.steps.empty());
}

TEST(AdvancedIntegrals, RejectsZeroLinearCoefficient) {
	EXPECT_THROW(AdvancedIntegrals::integrateBySubstitution(0.0, 1.0, 2, 0.0, 1.0), std::invalid_argument);
}
