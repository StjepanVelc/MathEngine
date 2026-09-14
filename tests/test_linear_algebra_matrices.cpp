#include "aksiomat/linear_algebra/Matrices.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::linear_algebra;

TEST(Matrices, AddsTwoMatrices) {
	MatrixRows a{{1, 2}, {3, 4}};
	MatrixRows b{{5, 6}, {7, 8}};
	MatrixOperationResult result = Matrices::add(a, b);
	EXPECT_DOUBLE_EQ(result.matrix[0][0], 6.0);
	EXPECT_DOUBLE_EQ(result.matrix[1][1], 12.0);
}

TEST(Matrices, MultipliesTwoMatrices) {
	MatrixRows a{{1, 2}, {3, 4}};
	MatrixRows identity{{1, 0}, {0, 1}};
	MatrixOperationResult result = Matrices::multiply(a, identity);
	EXPECT_DOUBLE_EQ(result.matrix[0][0], 1.0);
	EXPECT_DOUBLE_EQ(result.matrix[0][1], 2.0);
	EXPECT_DOUBLE_EQ(result.matrix[1][0], 3.0);
	EXPECT_DOUBLE_EQ(result.matrix[1][1], 4.0);
}

TEST(Matrices, ComputesDeterminant2x2) {
	MatrixRows m{{1, 2}, {3, 4}};
	DeterminantResult result = Matrices::determinant(m);
	EXPECT_DOUBLE_EQ(result.determinant, -2.0);
}

TEST(Matrices, ComputesDeterminant3x3) {
	MatrixRows m{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	DeterminantResult result = Matrices::determinant(m);
	EXPECT_DOUBLE_EQ(result.determinant, 1.0);
}

TEST(Matrices, ComputesInverseOfInvertibleMatrix) {
	MatrixRows m{{4, 7}, {2, 6}};
	InverseResult result = Matrices::inverse(m);
	ASSERT_TRUE(result.invertible);
	EXPECT_NEAR(result.inverse[0][0], 0.6, 1e-9);
	EXPECT_NEAR(result.inverse[0][1], -0.7, 1e-9);
	EXPECT_NEAR(result.inverse[1][0], -0.2, 1e-9);
	EXPECT_NEAR(result.inverse[1][1], 0.4, 1e-9);
}

TEST(Matrices, DetectsSingularMatrix) {
	MatrixRows m{{1, 2}, {2, 4}};
	InverseResult result = Matrices::inverse(m);
	EXPECT_FALSE(result.invertible);
}

TEST(Matrices, AppliesLinearTransformation) {
	MatrixRows m{{2, 0}, {0, 3}};
	TransformationResult result = Matrices::applyTransformation(m, {1, 1});
	EXPECT_DOUBLE_EQ(result.transformedVector[0], 2.0);
	EXPECT_DOUBLE_EQ(result.transformedVector[1], 3.0);
}

TEST(Matrices, RejectsNonSquareDimensions) {
	MatrixRows m{{1, 2, 3}, {4, 5, 6}};
	EXPECT_THROW(Matrices::determinant(m), std::invalid_argument);
}
