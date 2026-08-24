#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/trigonometry/GeneralTriangle.hpp"

using aksiomat::trigonometry::GeneralTriangle;

TEST(TrigonometryGeneralTriangle, SolvesSssTriangle) {
	const auto result = GeneralTriangle::fromSides(3.0, 4.0, 5.0);
	EXPECT_NEAR(result.angleA, 36.8698976458, 1e-9);
	EXPECT_NEAR(result.angleB, 53.1301023542, 1e-9);
	EXPECT_NEAR(result.angleC, 90.0, 1e-9);
	EXPECT_DOUBLE_EQ(result.area, 6.0);
}

TEST(TrigonometryGeneralTriangle, SolvesSasTriangleWithCosineLaw) {
	const auto result = GeneralTriangle::fromTwoSidesAndIncludedAngle(5.0, 7.0, 60.0);
	EXPECT_NEAR(result.sideC, std::sqrt(39.0), 1e-12);
	EXPECT_NEAR(result.area, 35.0 * std::sqrt(3.0) / 4.0, 1e-12);
	EXPECT_NEAR(result.angleA + result.angleB + result.angleC, 180.0, 1e-12);
}

TEST(TrigonometryGeneralTriangle, SolvesAasTriangleWithSineLaw) {
	const auto result = GeneralTriangle::fromSideAndTwoAngles(10.0, 30.0, 60.0);
	EXPECT_NEAR(result.angleC, 90.0, 1e-12);
	EXPECT_NEAR(result.sideB, 10.0 * std::sqrt(3.0), 1e-12);
	EXPECT_NEAR(result.sideC, 20.0, 1e-12);
	EXPECT_NEAR(result.area, 50.0 * std::sqrt(3.0), 1e-10);
}

TEST(TrigonometryGeneralTriangle, RejectsImpossibleSidesAndAngles) {
	EXPECT_THROW(GeneralTriangle::fromSides(1.0, 2.0, 3.0), std::invalid_argument);
	EXPECT_THROW(GeneralTriangle::fromTwoSidesAndIncludedAngle(5.0, 7.0, 180.0), std::invalid_argument);
	EXPECT_THROW(GeneralTriangle::fromSideAndTwoAngles(10.0, 100.0, 80.0), std::invalid_argument);
	EXPECT_THROW(GeneralTriangle::fromSideAndTwoAngles(-1.0, 30.0, 40.0), std::invalid_argument);
}
