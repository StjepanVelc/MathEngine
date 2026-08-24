#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/sequences/GeometricSequence.hpp"

using aksiomat::sequences::GeometricSequence;

TEST(GeometricSequence, CalculatesTermsFiniteSumsAndIndexes) {
	const GeometricSequence sequence(3.0, 2.0);
	EXPECT_DOUBLE_EQ(sequence.term(5), 48.0);
	EXPECT_DOUBLE_EQ(sequence.partialSum(5), 93.0);
	EXPECT_EQ(sequence.indexOf(48.0), 5u);
	EXPECT_FALSE(sequence.indexOf(50.0, 10).has_value());
	EXPECT_EQ(sequence.generate(4), (std::vector<double>{3.0, 6.0, 12.0, 24.0}));
}

TEST(GeometricSequence, HandlesSpecialRatiosAndInfiniteSeries) {
	EXPECT_DOUBLE_EQ(GeometricSequence(4.0, 1.0).partialSum(5), 20.0);
	EXPECT_EQ(GeometricSequence(4.0, 0.0).indexOf(0.0), 2u);
	EXPECT_EQ(GeometricSequence(2.0, -2.0).indexOf(-4.0), 2u);

	const auto convergent = GeometricSequence(8.0, 0.5).infiniteSum();
	EXPECT_TRUE(convergent.converges);
	ASSERT_TRUE(convergent.sum.has_value());
	EXPECT_DOUBLE_EQ(*convergent.sum, 16.0);
	EXPECT_FALSE(GeometricSequence(1.0, -1.0).infiniteSum().converges);
	EXPECT_FALSE(GeometricSequence(1.0, 1.2).infiniteSum().sum.has_value());
}

TEST(GeometricSequence, RejectsInvalidInput) {
	EXPECT_THROW(GeometricSequence(1.0, 2.0).term(0), std::invalid_argument);
	EXPECT_THROW(GeometricSequence(1.0, 2.0).partialSum(0), std::invalid_argument);
	EXPECT_THROW(GeometricSequence(1.0, 2.0).indexOf(8.0, 0), std::invalid_argument);
}
