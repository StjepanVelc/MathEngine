#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/IntegralApplications.hpp"

using aksiomat::integral_calculus::IntegralApplications;

namespace {
constexpr double kPi = 3.14159265358979323846;
}

TEST(IntegralApplications, VolumeOfRevolution) {
	const auto cone = IntegralApplications::volumeOfRevolution("x", 0, 3);
	9.0 * kPi
	const auto sphere = IntegralApplications::volumeOfRevolution("sqrt(4-x^2)", -2, 2);
	32.0 * kPi / 3.0
	EXPECT_FALSE(cone.steps.empty());
	EXPECT_THROW(IntegralApplications::volumeOfRevolution("x", 2, 1), std::invalid_argument);
}

TEST(IntegralApplications, ArcLength) {
	EXPECT_NEAR(IntegralApplications::arcLength("x", 0, 3).value, 3.0 * std::sqrt(2.0), 1e-6);
	EXPECT_NEAR(IntegralApplications::arcLength("x^2", 0, 1).value, 1.4789428575, 1e-5);
	EXPECT_NEAR(IntegralApplications::arcLength("cosh(x)", 0, 1).value, std::sinh(1.0), 1e-5);
}
