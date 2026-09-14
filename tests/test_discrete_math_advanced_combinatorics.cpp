#include "aksiomat/discrete_math/AdvancedCombinatorics.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::discrete_math;

TEST(AdvancedCombinatorics, ComputesUnionSizeTwoSets) {
	InclusionExclusionResult result = AdvancedCombinatorics::unionSizeTwoSets(5, 7, 2);
	EXPECT_EQ(result.unionSize, 10);
}

TEST(AdvancedCombinatorics, ComputesUnionSizeThreeSets) {
	InclusionExclusionResult result = AdvancedCombinatorics::unionSizeThreeSets(10, 10, 10, 3, 3, 3, 1);
	EXPECT_EQ(result.unionSize, 22);
}

TEST(AdvancedCombinatorics, ComputesPigeonhole) {
	PigeonholeResult result = AdvancedCombinatorics::pigeonhole(10, 3);
	EXPECT_EQ(result.minimumPerHole, 4);
}

TEST(AdvancedCombinatorics, ThrowsOnZeroHoles) {
	EXPECT_THROW(AdvancedCombinatorics::pigeonhole(10, 0), std::invalid_argument);
}

TEST(AdvancedCombinatorics, ComputesDerangementsOfFour) {
	DerangementResult result = AdvancedCombinatorics::derangements(4);
	EXPECT_EQ(result.derangements, 9);
}

TEST(AdvancedCombinatorics, ComputesDerangementsBaseCases) {
	EXPECT_EQ(AdvancedCombinatorics::derangements(0).derangements, 1);
	EXPECT_EQ(AdvancedCombinatorics::derangements(1).derangements, 0);
}

TEST(AdvancedCombinatorics, ThrowsOnNegativeN) {
	EXPECT_THROW(AdvancedCombinatorics::derangements(-1), std::invalid_argument);
}
