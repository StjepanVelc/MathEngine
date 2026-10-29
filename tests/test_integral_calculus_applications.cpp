#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/IntegralApplications.hpp"

using aksiomat::integral_calculus::IntegralApplications;

namespace {
constexpr double M_PI = 3.14159265358979323846;
}

TEST(IntegralApplications, VolumeOfRevolution) {
	const auto cone = IntegralApplications::volumeOfRevolution("x", 0, 3);
	EXPECT_NEAR(cone.value, 9.0 * M_PI, 1e-6);
	const auto sphere = IntegralApplications::volumeOfRevolution("sqrt(4-x^2)", -2, 2);
	EXPECT_NEAR(sphere.value, 32.0 * M_PI / 3.0, 1e-3);
	EXPECT_FALSE(cone.steps.empty());
	EXPECT_THROW(IntegralApplications::volumeOfRevolution("x", 2, 1), std::invalid_argument);
}

TEST(IntegralApplications, ArcLength) {
	EXPECT_NEAR(IntegralApplications::arcLength("x", 0, 3).value, 3.0 * std::sqrt(2.0), 1e-6);
	EXPECT_NEAR(IntegralApplications::arcLength("x^2", 0, 1).value, 1.4789428575, 1e-5);
	EXPECT_NEAR(IntegralApplications::arcLength("cosh(x)", 0, 1).value, std::sinh(1.0), 1e-5);
}
