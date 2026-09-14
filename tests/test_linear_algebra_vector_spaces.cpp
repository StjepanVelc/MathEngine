#include "aksiomat/linear_algebra/VectorSpaces.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::linear_algebra;

TEST(VectorSpaces, DetectsIndependentVectors) {
	std::vector<std::vector<double>> vectors{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	LinearIndependenceResult result = VectorSpaces::checkLinearIndependence(vectors);
	EXPECT_TRUE(result.linearlyIndependent);
	EXPECT_EQ(result.rank, 3);
}

TEST(VectorSpaces, DetectsDependentVectors) {
	std::vector<std::vector<double>> vectors{{1, 2, 3}, {2, 4, 6}};
	LinearIndependenceResult result = VectorSpaces::checkLinearIndependence(vectors);
	EXPECT_FALSE(result.linearlyIndependent);
	EXPECT_EQ(result.rank, 1);
}

TEST(VectorSpaces, ComputesRank) {
	std::vector<std::vector<double>> vectors{{1, 1}, {2, 2}, {0, 1}};
	RankResult result = VectorSpaces::computeRank(vectors);
	EXPECT_EQ(result.rank, 2);
}

TEST(VectorSpaces, ExtractsBasis) {
	std::vector<std::vector<double>> vectors{{1, 0}, {2, 0}, {0, 1}};
	BasisResult result = VectorSpaces::extractBasis(vectors);
	EXPECT_EQ(result.dimension, 2);
	EXPECT_EQ(result.basisVectors.size(), 2u);
}

TEST(VectorSpaces, RejectsMismatchedDimensions) {
	std::vector<std::vector<double>> vectors{{1, 0}, {0, 1, 2}};
	EXPECT_THROW(VectorSpaces::checkLinearIndependence(vectors), std::invalid_argument);
}
