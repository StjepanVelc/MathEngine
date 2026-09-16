#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/analytic_geometry/LinesDistances.hpp"
#include "aksiomat/analytic_geometry/PointsVectors.hpp"

using namespace aksiomat::analytic_geometry;

TEST(AnalyticGeometryVectors, CalculatesOperationsMagnitudeAndAngles) {
	const auto vector = PointsVectors::fromPoints({1, 2}, {4, 6});
	EXPECT_DOUBLE_EQ(vector.x, 3); EXPECT_DOUBLE_EQ(vector.y, 4);
	EXPECT_DOUBLE_EQ(PointsVectors::magnitude(vector), 5);
	EXPECT_DOUBLE_EQ(PointsVectors::dot({1, 0}, {0, 2}), 0);
	EXPECT_DOUBLE_EQ(PointsVectors::determinant({1, 0}, {0, 2}), 2);
	EXPECT_NEAR(PointsVectors::angleDegrees({1, 0}, {0, 2}), 90, 1e-12);
	EXPECT_NEAR(PointsVectors::analyze({0, -2}).directionDegrees, 270, 1e-12);
}

TEST(AnalyticGeometryVectors, RejectsZeroAndNonFiniteVectors) {
	EXPECT_THROW(PointsVectors::angleDegrees({0, 0}, {1, 0}), std::invalid_argument);
	EXPECT_THROW(PointsVectors::analyze({0, 0}), std::invalid_argument);
	EXPECT_THROW(PointsVectors::magnitude({std::numeric_limits<double>::infinity(), 0}), std::invalid_argument);
}

TEST(AnalyticGeometryVectors, RejectsCoordinatesAboveMagnitudeLimit) {
	EXPECT_THROW(PointsVectors::magnitude({2e6, 0}), std::invalid_argument);
	EXPECT_NO_THROW(PointsVectors::magnitude({1e6, 0}));
}

TEST(AnalyticGeometryLines, HandlesGeneralAndVerticalLines) {
	const auto diagonal = LinesDistances::throughPoints({0, 0}, {2, 2});
	const auto analysis = LinesDistances::analyze(diagonal);
	ASSERT_TRUE(analysis.slope); EXPECT_NEAR(*analysis.slope, 1, 1e-12);
	const auto vertical = LinesDistances::analyze(LinesDistances::throughPoints({3, -2}, {3, 5}));
	EXPECT_TRUE(vertical.vertical); EXPECT_FALSE(vertical.slope); ASSERT_TRUE(vertical.xIntercept); EXPECT_NEAR(*vertical.xIntercept, 3, 1e-12);
}

TEST(AnalyticGeometryLines, ClassifiesRelationsAndIntersection) {
	const auto first = LinesDistances::fromSlopeIntercept(1, 0);
	const auto second = LinesDistances::fromSlopeIntercept(-1, 2);
	const auto relation = LinesDistances::relation(first, second);
	EXPECT_EQ(relation.relation, LineRelation::Perpendicular); ASSERT_TRUE(relation.intersection);
	EXPECT_NEAR(relation.intersection->x, 1, 1e-12); EXPECT_NEAR(relation.intersection->y, 1, 1e-12);
	EXPECT_EQ(LinesDistances::relation(first, LinesDistances::fromSlopeIntercept(1, 3)).relation, LineRelation::Parallel);
	EXPECT_EQ(LinesDistances::relation(first, LinesDistances::throughPoints({2, 2}, {4, 4})).relation, LineRelation::Coincident);
}

TEST(AnalyticGeometryLines, CalculatesDistancesAndRejectsDegenerateData) {
	EXPECT_DOUBLE_EQ(LinesDistances::pointDistance({0, 0}, {3, 4}), 5);
	EXPECT_NEAR(LinesDistances::pointToLineDistance({0, 2}, LinesDistances::fromSlopeIntercept(0, 0)), 2, 1e-12);
	EXPECT_THROW(LinesDistances::throughPoints({1, 1}, {1, 1}), std::invalid_argument);
	EXPECT_THROW(LinesDistances::pointDirection({0, 0}, {0, 0}), std::invalid_argument);
	EXPECT_THROW(LinesDistances::analyze({0, 0, 1}), std::invalid_argument);
}
