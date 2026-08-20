#include <gtest/gtest.h>

#include <limits>
#include <numbers>
#include <stdexcept>

#include "aksiomat/geometry/Coordinates.hpp"
#include "aksiomat/geometry/Solids.hpp"

using namespace aksiomat::geometry;

TEST(GeometrySolids, CalculatesCubeAndCuboid) {
	const auto cube = Solids::cube(3.0);
	EXPECT_DOUBLE_EQ(cube.surfaceArea, 54.0);
	EXPECT_DOUBLE_EQ(cube.volume, 27.0);

	const auto cuboid = Solids::cuboid(2.0, 3.0, 4.0);
	EXPECT_DOUBLE_EQ(cuboid.surfaceArea, 52.0);
	EXPECT_DOUBLE_EQ(cuboid.volume, 24.0);
}

TEST(GeometrySolids, CalculatesPrismAndCylinder) {
	const auto prism = Solids::prism(6.0, 12.0, 10.0);
	EXPECT_DOUBLE_EQ(prism.surfaceArea, 132.0);
	EXPECT_DOUBLE_EQ(prism.volume, 60.0);

	const auto cylinder = Solids::cylinder(2.0, 5.0);
	EXPECT_DOUBLE_EQ(cylinder.surfaceArea, 28.0 * std::numbers::pi);
	EXPECT_DOUBLE_EQ(cylinder.volume, 20.0 * std::numbers::pi);
}

TEST(GeometrySolids, RejectsInvalidMeasurements) {
	EXPECT_THROW(Solids::cube(0.0), std::invalid_argument);
	EXPECT_THROW(Solids::cuboid(1.0, -2.0, 3.0), std::invalid_argument);
	EXPECT_THROW(Solids::cylinder(2.0, std::numeric_limits<double>::infinity()), std::invalid_argument);
}

TEST(GeometryCoordinates, CalculatesDistanceAndMidpoint) {
	EXPECT_DOUBLE_EQ(Coordinates::distance({0.0, 0.0}, {3.0, 4.0}), 5.0);
	const auto middle = Coordinates::midpoint({-2.0, 4.0}, {6.0, -2.0});
	EXPECT_DOUBLE_EQ(middle.x, 2.0);
	EXPECT_DOUBLE_EQ(middle.y, 1.0);
}

TEST(GeometryCoordinates, RejectsNonFiniteCoordinates) {
	const auto infinity = std::numeric_limits<double>::infinity();
	EXPECT_THROW(Coordinates::distance({infinity, 0.0}, {1.0, 2.0}), std::invalid_argument);
}
