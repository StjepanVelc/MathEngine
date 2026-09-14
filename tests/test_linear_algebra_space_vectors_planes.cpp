#include "aksiomat/linear_algebra/SpaceVectorsPlanes.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::linear_algebra;

TEST(SpaceVectorsPlanes, ComputesDotAndCrossProduct) {
	Vector3D a{1, 0, 0};
	Vector3D b{0, 1, 0};
	EXPECT_DOUBLE_EQ(SpaceVectorsPlanes::dot(a, b), 0.0);
	CrossProductResult cross = SpaceVectorsPlanes::cross(a, b);
	EXPECT_DOUBLE_EQ(cross.crossProduct.x, 0.0);
	EXPECT_DOUBLE_EQ(cross.crossProduct.y, 0.0);
	EXPECT_DOUBLE_EQ(cross.crossProduct.z, 1.0);
	EXPECT_DOUBLE_EQ(cross.parallelepipedVolumeHint, 1.0);
}

TEST(SpaceVectorsPlanes, AnalyzesVectorMagnitude) {
	VectorAnalysisResult result = SpaceVectorsPlanes::analyze(Vector3D{3, 4, 0});
	EXPECT_DOUBLE_EQ(result.magnitude, 5.0);
	EXPECT_FALSE(result.steps.empty());
}

TEST(SpaceVectorsPlanes, ComputesDistancePointToPlane) {
	// Ravnina z = 0 (0x + 0y + 1z + 0 = 0), tocka (0,0,5).
	const double distance = SpaceVectorsPlanes::distancePointToPlane(Point3D{0, 0, 5}, 0, 0, 1, 0);
	EXPECT_DOUBLE_EQ(distance, 5.0);
}

TEST(SpaceVectorsPlanes, BuildsPlaneFromThreePoints) {
	PlaneFromPointsResult plane = SpaceVectorsPlanes::planeFromThreePoints(Point3D{0, 0, 0}, Point3D{1, 0, 0}, Point3D{0, 1, 0});
	// Ocekivana ravnina z = 0 (normala paralelna s (0,0,c)).
	EXPECT_DOUBLE_EQ(plane.normalX, 0.0);
	EXPECT_DOUBLE_EQ(plane.normalY, 0.0);
	EXPECT_NE(plane.normalZ, 0.0);
}

TEST(SpaceVectorsPlanes, RejectsCollinearPointsForPlane) {
	EXPECT_THROW(SpaceVectorsPlanes::planeFromThreePoints(Point3D{0, 0, 0}, Point3D{1, 1, 1}, Point3D{2, 2, 2}), std::invalid_argument);
}

TEST(SpaceVectorsPlanes, FindsLinePlaneIntersection) {
	// Pravac kroz (0,0,0) u smjeru (0,0,1), ravnina z - 5 = 0.
	LinePlaneRelation relation = SpaceVectorsPlanes::lineVsPlane(Point3D{0, 0, 0}, Vector3D{0, 0, 1}, 0, 0, 1, -5);
	ASSERT_TRUE(relation.intersects);
	ASSERT_TRUE(relation.intersectionPoint.has_value());
	EXPECT_NEAR(relation.intersectionPoint->z, 5.0, 1e-9);
}

TEST(SpaceVectorsPlanes, DetectsParallelLineAndPlane) {
	LinePlaneRelation relation = SpaceVectorsPlanes::lineVsPlane(Point3D{0, 0, 1}, Vector3D{1, 0, 0}, 0, 0, 1, 0);
	EXPECT_TRUE(relation.lineParallelToPlane);
	EXPECT_FALSE(relation.intersects);
}
