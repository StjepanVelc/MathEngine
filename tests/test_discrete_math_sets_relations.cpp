#include "aksiomat/discrete_math/SetsRelations.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::discrete_math;

TEST(SetsRelations, ComputesUnion) {
	SetOperationResult result = SetsRelations::unionOf({1, 2, 3}, {3, 4});
	IntSet expected{1, 2, 3, 4};
	EXPECT_EQ(result.result, expected);
}

TEST(SetsRelations, ComputesIntersection) {
	SetOperationResult result = SetsRelations::intersectionOf({1, 2, 3}, {2, 3, 4});
	IntSet expected{2, 3};
	EXPECT_EQ(result.result, expected);
}

TEST(SetsRelations, ComputesDifference) {
	SetOperationResult result = SetsRelations::differenceOf({1, 2, 3}, {2, 3});
	IntSet expected{1};
	EXPECT_EQ(result.result, expected);
}

TEST(SetsRelations, ComputesSymmetricDifference) {
	SetOperationResult result = SetsRelations::symmetricDifferenceOf({1, 2, 3}, {2, 3, 4});
	IntSet expected{1, 4};
	EXPECT_EQ(result.result, expected);
}

TEST(SetsRelations, IdentifiesEquivalenceRelation) {
	IntSet domain{1, 2, 3};
	IntPairs pairs{{1,1},{2,2},{3,3},{1,2},{2,1}};
	RelationPropertiesResult result = SetsRelations::analyzeRelation(domain, pairs);
	EXPECT_TRUE(result.reflexive);
	EXPECT_TRUE(result.symmetric);
	EXPECT_TRUE(result.transitive);
	EXPECT_TRUE(result.isEquivalence);
	EXPECT_FALSE(result.isPartialOrder);
}

TEST(SetsRelations, IdentifiesPartialOrder) {
	IntSet domain{1, 2, 3};
	IntPairs pairs{{1,1},{2,2},{3,3},{1,2},{1,3},{2,3}};
	RelationPropertiesResult result = SetsRelations::analyzeRelation(domain, pairs);
	EXPECT_TRUE(result.reflexive);
	EXPECT_TRUE(result.antisymmetric);
	EXPECT_TRUE(result.transitive);
	EXPECT_TRUE(result.isPartialOrder);
	EXPECT_FALSE(result.isEquivalence);
}

TEST(SetsRelations, ThrowsOnEmptyDomain) {
	EXPECT_THROW(SetsRelations::analyzeRelation({}, {}), std::invalid_argument);
}
