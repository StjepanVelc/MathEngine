#include <gtest/gtest.h>

#include <limits>
#include <numbers>
#include <stdexcept>

#include "aksiomat/trigonometry/Angles.hpp"

using namespace aksiomat::trigonometry;

TEST(TrigonometryAngles, ConvertsDegreesAndRadians) {
	EXPECT_DOUBLE_EQ(Angles::toRadians(180.0), std::numbers::pi);
	EXPECT_DOUBLE_EQ(Angles::toDegrees(std::numbers::pi / 2.0), 90.0);
}

TEST(TrigonometryAngles, NormalizesPositiveAndNegativeTurns) {
	EXPECT_DOUBLE_EQ(Angles::normalizeDegrees(450.0), 90.0);
	EXPECT_DOUBLE_EQ(Angles::normalizeDegrees(-30.0), 330.0);
	EXPECT_NEAR(Angles::normalizeRadians(5.0 * std::numbers::pi), std::numbers::pi, 1e-12);
}

TEST(TrigonometryAngles, LocatesPointAndQuadrantOnUnitCircle) {
	const auto point = Angles::unitCircle(150.0);
	EXPECT_DOUBLE_EQ(point.degrees, 150.0);
	EXPECT_NEAR(point.x, -std::sqrt(3.0) / 2.0, 1e-12);
	EXPECT_NEAR(point.y, 0.5, 1e-12);
	EXPECT_EQ(point.quadrant, 2);
	EXPECT_DOUBLE_EQ(point.referenceDegrees, 30.0);
}

TEST(TrigonometryAngles, IdentifiesAxesWithoutQuadrant) {
	const auto point = Angles::unitCircle(270.0);
	EXPECT_DOUBLE_EQ(point.x, 0.0);
	EXPECT_DOUBLE_EQ(point.y, -1.0);
	EXPECT_EQ(point.quadrant, 0);
	EXPECT_DOUBLE_EQ(point.referenceDegrees, 90.0);
}

TEST(TrigonometryAngles, AcceptsRadiansAndRejectsInvalidInputs) {
	const auto point = Angles::unitCircle(std::numbers::pi / 4.0, AngleUnit::Radians);
	EXPECT_NEAR(point.degrees, 45.0, 1e-12);
	EXPECT_THROW(Angles::toRadians(std::numeric_limits<double>::infinity()), std::invalid_argument);
	EXPECT_THROW(Angles::unitCircle(1.0, static_cast<AngleUnit>(99)), std::invalid_argument);
}
