#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/sequences/ArithmeticSequence.hpp"
#include "aksiomat/sequences/SequenceAnalysis.hpp"

using namespace aksiomat::sequences;

TEST(SequenceAnalysis, ClassifiesAndBuildsGraphPoints) {
	const auto increasing = SequenceAnalysis::analyze({1.0, 2.0, 4.0});
	EXPECT_EQ(increasing.monotonicity, Monotonicity::StrictlyIncreasing);
	EXPECT_EQ(increasing.points.size(), 3u);
	EXPECT_EQ(increasing.points[2].index, 3u);
	EXPECT_DOUBLE_EQ(increasing.differences[1], 2.0);
	EXPECT_DOUBLE_EQ(increasing.minimum, 1.0);
	EXPECT_DOUBLE_EQ(increasing.maximum, 4.0);

	EXPECT_EQ(SequenceAnalysis::analyze({2.0, 2.0, 3.0}).monotonicity, Monotonicity::NonDecreasing);
	EXPECT_EQ(SequenceAnalysis::analyze({3.0, 2.0, 2.0}).monotonicity, Monotonicity::NonIncreasing);
	EXPECT_EQ(SequenceAnalysis::analyze({3.0, 2.0, 1.0}).monotonicity, Monotonicity::StrictlyDecreasing);
	EXPECT_EQ(SequenceAnalysis::analyze({2.0, 2.0}).monotonicity, Monotonicity::Constant);
	EXPECT_EQ(SequenceAnalysis::analyze({1.0, 3.0, 2.0}).monotonicity, Monotonicity::NotMonotonic);
}

TEST(SequenceAnalysis, RejectsEmptyAndNonFiniteTerms) {
	EXPECT_THROW(SequenceAnalysis::analyze({}), std::invalid_argument);
	EXPECT_THROW(SequenceAnalysis::analyze({1.0, std::numeric_limits<double>::infinity()}), std::invalid_argument);
}

TEST(ArithmeticSequence, CalculatesTermsSumsAndIndexes) {
	const ArithmeticSequence sequence(3.0, 4.0);
	EXPECT_DOUBLE_EQ(sequence.term(6), 23.0);
	EXPECT_DOUBLE_EQ(sequence.partialSum(6), 78.0);
	ASSERT_TRUE(sequence.indexOf(23.0).has_value());
	EXPECT_EQ(*sequence.indexOf(23.0), 6u);
	EXPECT_FALSE(sequence.indexOf(24.0).has_value());
	EXPECT_EQ(sequence.generate(4), (std::vector<double>{3.0, 7.0, 11.0, 15.0}));
}

TEST(ArithmeticSequence, HandlesConstantAndRejectsInvalidInput) {
	const ArithmeticSequence constant(5.0, 0.0);
	EXPECT_EQ(constant.indexOf(5.0), 1u);
	EXPECT_FALSE(constant.indexOf(6.0).has_value());
	EXPECT_THROW(constant.term(0), std::invalid_argument);
	EXPECT_THROW(constant.generate(0), std::invalid_argument);
	EXPECT_THROW(ArithmeticSequence(std::numeric_limits<double>::infinity(), 1.0), std::invalid_argument);
}
