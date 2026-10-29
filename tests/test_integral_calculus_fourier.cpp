#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/FourierSeries.hpp"

using aksiomat::integral_calculus::FourierSeries;

namespace {
constexpr double kPi = 3.14159265358979323846;
}

TEST(FourierSeries, SineIsItsOwnSeries) {
	const auto r = FourierSeries::compute("sin(x)", kPi, 5);
	EXPECT_NEAR(r.b[0], 1.0, 1e-6);
	EXPECT_NEAR(r.a[0], 0.0, 1e-6);
	EXPECT_NEAR(r.b[1], 0.0, 1e-6);
	EXPECT_NEAR(r.meanSquareError, 0.0, 1e-10);
}

TEST(FourierSeries, SawtoothCoefficients) {
	// f(x) = x na [-pi, pi]: b_n = 2(-1)^(n+1)/n, a_n = 0.
	const auto r = FourierSeries::compute("x", kPi, 10);
	EXPECT_NEAR(r.b[0], 2.0, 1e-5);
	EXPECT_NEAR(r.b[1], -1.0, 1e-5);
	EXPECT_NEAR(r.b[2], 2.0 / 3.0, 1e-5);
	EXPECT_NEAR(r.a[3], 0.0, 1e-6);
	EXPECT_NEAR(FourierSeries::partialSum(r, 1.0), 1.0, 0.25);
}

TEST(FourierSeries, ConstantAndInvalid) {
	const auto r = FourierSeries::compute("3", 2.0, 3);
	EXPECT_NEAR(r.a0, 6.0, 1e-9);
	EXPECT_FALSE(r.steps.empty());
	EXPECT_THROW(FourierSeries::compute("x", -1.0, 3), std::invalid_argument);
	EXPECT_THROW(FourierSeries::compute("x", 1.0, 0), std::invalid_argument);
	EXPECT_THROW(FourierSeries::compute("1/x", 1.0, 3), std::invalid_argument);
}
