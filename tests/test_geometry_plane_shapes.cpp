#include <gtest/gtest.h>

#include <numbers>
#include <stdexcept>

#include "aksiomat/geometry/PlaneShapes.hpp"

using aksiomat::geometry::PlaneShapes;

TEST(GeometryPlaneShapes, CalculatesSquareAndRectangle) {
	const auto square = PlaneShapes::square(5.0);
	EXPECT_DOUBLE_EQ(square.perimeter, 20.0);
	EXPECT_DOUBLE_EQ(square.area, 25.0);

	const auto rectangle = PlaneShapes::rectangle(7.0, 3.0);
	EXPECT_DOUBLE_EQ(rectangle.perimeter, 20.0);
	EXPECT_DOUBLE_EQ(rectangle.area, 21.0);
}

TEST(GeometryPlaneShapes, CalculatesTriangleWithHeronFormula) {
	const auto triangle = PlaneShapes::triangle(3.0, 4.0, 5.0);
	EXPECT_DOUBLE_EQ(triangle.perimeter, 12.0);
	EXPECT_DOUBLE_EQ(triangle.area, 6.0);
}

TEST(GeometryPlaneShapes, CalculatesParallelogramAndTrapezoid) {
	const auto parallelogram = PlaneShapes::parallelogram(8.0, 5.0, 4.0);
	EXPECT_DOUBLE_EQ(parallelogram.perimeter, 26.0);
	EXPECT_DOUBLE_EQ(parallelogram.area, 32.0);

	const auto trapezoid = PlaneShapes::trapezoid(10.0, 6.0, 5.0, 5.0, 4.0);
	EXPECT_DOUBLE_EQ(trapezoid.perimeter, 26.0);
	EXPECT_DOUBLE_EQ(trapezoid.area, 32.0);
}

TEST(GeometryPlaneShapes, CalculatesCircleWithStandardPi) {
	const auto circle = PlaneShapes::circle(2.0);
	EXPECT_DOUBLE_EQ(circle.perimeter, 4.0 * std::numbers::pi);
	EXPECT_DOUBLE_EQ(circle.area, 4.0 * std::numbers::pi);
}

TEST(GeometryPlaneShapes, RejectsInvalidMeasurementsAndTriangles) {
	EXPECT_THROW(PlaneShapes::square(0.0), std::invalid_argument);
	EXPECT_THROW(PlaneShapes::rectangle(-2.0, 3.0), std::invalid_argument);
	EXPECT_THROW(PlaneShapes::triangle(1.0, 2.0, 3.0), std::invalid_argument);
	EXPECT_THROW(PlaneShapes::circle(-1.0), std::invalid_argument);
}
