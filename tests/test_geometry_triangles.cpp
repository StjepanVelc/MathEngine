#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/geometry/Triangles.hpp"

using aksiomat::geometry::Triangles;

TEST(GeometryTriangles, ClassifiesBySidesAndAngles) {
	const auto equilateral = Triangles::classify(5.0, 5.0, 5.0);
	EXPECT_EQ(equilateral.bySides, "jednakostranican");
	EXPECT_EQ(equilateral.byAngles, "ostrokutan");

	const auto right = Triangles::classify(3.0, 4.0, 5.0);
	EXPECT_EQ(right.bySides, "raznostranican");
	EXPECT_EQ(right.byAngles, "pravokutan");

	const auto obtuse = Triangles::classify(3.0, 3.0, 5.0);
	EXPECT_EQ(obtuse.bySides, "jednakokracan");
	EXPECT_EQ(obtuse.byAngles, "tupokutan");
}

TEST(GeometryTriangles, CalculatesMissingAngle) {
	EXPECT_DOUBLE_EQ(Triangles::thirdAngle(45.0, 65.0), 70.0);
	EXPECT_THROW(Triangles::thirdAngle(100.0, 80.0), std::invalid_argument);
}

TEST(GeometryTriangles, AppliesPythagoreanTheorem) {
	EXPECT_DOUBLE_EQ(Triangles::hypotenuse(3.0, 4.0), 5.0);
	EXPECT_DOUBLE_EQ(Triangles::leg(13.0, 5.0), 12.0);
}

TEST(GeometryTriangles, RejectsImpossibleInputs) {
	EXPECT_THROW(Triangles::classify(1.0, 2.0, 3.0), std::invalid_argument);
	EXPECT_THROW(Triangles::hypotenuse(-3.0, 4.0), std::invalid_argument);
	EXPECT_THROW(Triangles::leg(5.0, 5.0), std::invalid_argument);
}
