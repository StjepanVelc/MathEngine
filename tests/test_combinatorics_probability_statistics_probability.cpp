#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/combinatorics_probability_statistics/ProbabilityBasics.hpp"

using namespace aksiomat::combinatorics_probability_statistics;

TEST(ProbabilityBasics, ComputesClassicalProbability) {
	EXPECT_DOUBLE_EQ(ProbabilityBasics::classicalProbability(3, 6), 0.5);
	EXPECT_DOUBLE_EQ(ProbabilityBasics::classicalProbability(0, 6), 0.0);
	EXPECT_THROW(ProbabilityBasics::classicalProbability(7, 6), std::invalid_argument);
	EXPECT_THROW(ProbabilityBasics::classicalProbability(1, 0), std::invalid_argument);
}

TEST(ProbabilityBasics, ComputesComplementProbability) {
	EXPECT_DOUBLE_EQ(ProbabilityBasics::complementProbability(0.3), 0.7);
	EXPECT_THROW(ProbabilityBasics::complementProbability(1.5), std::invalid_argument);
	EXPECT_THROW(ProbabilityBasics::complementProbability(-0.1), std::invalid_argument);
}

TEST(ProbabilityBasics, ComputesUnionProbability) {
	EXPECT_DOUBLE_EQ(ProbabilityBasics::unionProbability(0.5, 0.4, 0.2), 0.7);
	EXPECT_THROW(ProbabilityBasics::unionProbability(0.2, 0.3, 0.5), std::invalid_argument);
}

TEST(ProbabilityBasics, ComputesConditionalProbability) {
	EXPECT_DOUBLE_EQ(ProbabilityBasics::conditionalProbability({0.2, 0.5}), 0.4);
	EXPECT_THROW(ProbabilityBasics::conditionalProbability({0.5, 0.0}), std::invalid_argument);
	EXPECT_THROW(ProbabilityBasics::conditionalProbability({0.6, 0.5}), std::invalid_argument);
}

TEST(ProbabilityBasics, ComputesIndependentEventsProbability) {
	EXPECT_DOUBLE_EQ(ProbabilityBasics::independentEventsProbability(0.5, 0.4), 0.2);
	EXPECT_THROW(ProbabilityBasics::independentEventsProbability(1.5, 0.4), std::invalid_argument);
}
