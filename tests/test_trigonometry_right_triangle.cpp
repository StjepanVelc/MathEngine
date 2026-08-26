#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/trigonometry/RightTriangle.hpp"

using namespace aksiomat::trigonometry;

TEST(TrigonometryRightTriangle, SolvesFromTwoLegs) {
	const auto result = RightTriangle::fromLegs(3.0, 4.0);
	EXPECT_DOUBLE_EQ(result.hypotenuse, 5.0);
	EXPECT_NEAR(result.angleDegrees, 36.8698976458, 1e-9);
	EXPECT_NEAR(result.complementaryAngleDegrees, 53.1301023542, 1e-9);
	EXPECT_FALSE(result.steps.empty());
}

TEST(TrigonometryRightTriangle, SolvesFromOppositeLegAndHypotenuse) {
	const auto result = RightTriangle::fromLegAndHypotenuse(5.0, 13.0, KnownSide::Opposite);
	EXPECT_DOUBLE_EQ(result.opposite, 5.0);
	EXPECT_DOUBLE_EQ(result.adjacent, 12.0);
	EXPECT_DOUBLE_EQ(result.hypotenuse, 13.0);
}

TEST(TrigonometryRightTriangle, SolvesFromAdjacentLegAndHypotenuse) {
	const auto result = RightTriangle::fromLegAndHypotenuse(12.0, 13.0, KnownSide::Adjacent);
	EXPECT_DOUBLE_EQ(result.opposite, 5.0);
	EXPECT_DOUBLE_EQ(result.adjacent, 12.0);
}

TEST(TrigonometryRightTriangle, SolvesFromHypotenuseAndAngle) {
	const auto result = RightTriangle::fromSideAndAngle(10.0, KnownSide::Hypotenuse, 30.0);
	EXPECT_NEAR(result.opposite, 5.0, 1e-12);
	EXPECT_NEAR(result.adjacent, 5.0 * std::sqrt(3.0), 1e-12);
	EXPECT_DOUBLE_EQ(result.hypotenuse, 10.0);
	EXPECT_DOUBLE_EQ(result.angleDegrees, 30.0);
}

TEST(TrigonometryRightTriangle, SolvesFromLegAndAngle) {
	const auto result = RightTriangle::fromSideAndAngle(4.0, KnownSide::Adjacent, 45.0);
	EXPECT_NEAR(result.opposite, 4.0, 1e-12);
	EXPECT_NEAR(result.hypotenuse, 4.0 * std::sqrt(2.0), 1e-12);
}

TEST(TrigonometryRightTriangle, RejectsImpossibleOrInsufficientData) {
	EXPECT_THROW(RightTriangle::fromLegs(0.0, 4.0), std::invalid_argument);
	EXPECT_THROW(RightTriangle::fromLegAndHypotenuse(5.0, 5.0, KnownSide::Opposite), std::invalid_argument);
	EXPECT_THROW(RightTriangle::fromLegAndHypotenuse(3.0, 5.0, KnownSide::Hypotenuse), std::invalid_argument);
	EXPECT_THROW(RightTriangle::fromSideAndAngle(3.0, KnownSide::Adjacent, 90.0), std::invalid_argument);
}
