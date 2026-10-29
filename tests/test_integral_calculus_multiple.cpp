#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/MultipleIntegrals.hpp"

using aksiomat::integral_calculus::MultipleIntegrals;

namespace {
constexpr double kPi = 3.14159265358979323846;
}

TEST(MultipleIntegrals, DoubleKnownValues) {
	// int_0^1 int_0^2 x*y dx dy = (2) * (1/2) = 1
	EXPECT_NEAR(MultipleIntegrals::doubleIntegral("x*y", 0, 2, 0, 1).value, 1.0, 1e-9);
	// int_0^1 int_0^1 (x^2 + y^2) = 2/3
	EXPECT_NEAR(MultipleIntegrals::doubleIntegral("x^2 + y^2", 0, 1, 0, 1).value, 2.0 / 3.0, 1e-9);
	// int_0^pi int_0^pi sin(x)*sin(y) = 4
	EXPECT_NEAR(MultipleIntegrals::doubleIntegral("sin(x)*sin(y)", 0, kPi, 0, kPi, 60).value, 4.0, 1e-5);
}

TEST(MultipleIntegrals, TripleKnownValues) {
	EXPECT_NEAR(MultipleIntegrals::tripleIntegral("x*y*z", 0, 1, 0, 1, 0, 1).value, 0.125, 1e-9);
	const auto r = MultipleIntegrals::tripleIntegral("1", 0, 2, 0, 2, 0, 2);
	EXPECT_NEAR(r.value, 8.0, 1e-9);
	EXPECT_NEAR(r.volume, 8.0, 1e-12);
	EXPECT_NEAR(r.mean, 1.0, 1e-9);
	EXPECT_FALSE(r.steps.empty());
}

TEST(MultipleIntegrals, InvalidInput) {
	EXPECT_THROW(MultipleIntegrals::doubleIntegral("x*y", 1, 0, 0, 1), std::invalid_argument);
	EXPECT_THROW(MultipleIntegrals::doubleIntegral("1/(x-y)", 0, 1, 0, 1), std::invalid_argument);
	EXPECT_THROW(MultipleIntegrals::tripleIntegral("x+w", 0, 1, 0, 1, 0, 1), std::invalid_argument);
}

