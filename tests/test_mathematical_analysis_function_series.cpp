#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/mathematical_analysis/FunctionSeries.hpp"

using aksiomat::mathematical_analysis::FunctionSeries;

TEST(FunctionSeries, BuildsTaylorSeriesForExponentialAtZero) {
	// f(x) = e^x, f^(n)(x) = e^x for all n
	const auto derivativeAt = [](int, double x) { return std::exp(x); };
	const auto actual = [](double x) { return std::exp(x); };
	const auto result = FunctionSeries::buildTaylorSeries(derivativeAt, actual, "e^x", 0.0, 5, 1.0);
	EXPECT_NEAR(result.taylorApproximationAtPoint, std::exp(1.0), 0.01);
	EXPECT_NEAR(result.actualValueAtPoint, std::exp(1.0), 1e-9);
	EXPECT_LT(result.approximationError, 0.01);
	EXPECT_EQ(result.coefficients.size(), 6u);
	EXPECT_FALSE(result.steps.empty());
}

TEST(FunctionSeries, RejectsNegativeOrder) {
	const auto derivativeAt = [](int, double) { return 0.0; };
	const auto actual = [](double) { return 0.0; };
	EXPECT_THROW(FunctionSeries::buildTaylorSeries(derivativeAt, actual, "f", 0.0, -1, 1.0), std::invalid_argument);
}

TEST(FunctionSeries, AnalyzesConvergentPowerSeries) {
	// coefficients corresponding to geometric-like series 1, 1, 1, 1 => ratio test gives R = 1
	const auto result = FunctionSeries::analyzePowerSeries({1.0, 1.0, 1.0, 1.0}, 0.5);
	EXPECT_NEAR(result.radiusOfConvergence, 1.0, 1e-9);
	EXPECT_TRUE(result.convergesAtPoint);
	EXPECT_FALSE(result.steps.empty());
}

TEST(FunctionSeries, DetectsDivergenceOutsideRadius) {
	const auto result = FunctionSeries::analyzePowerSeries({1.0, 1.0, 1.0, 1.0}, 2.0);
	EXPECT_FALSE(result.convergesAtPoint);
}

TEST(FunctionSeries, RejectsEmptyCoefficients) {
	EXPECT_THROW(FunctionSeries::analyzePowerSeries({}, 1.0), std::invalid_argument);
}
