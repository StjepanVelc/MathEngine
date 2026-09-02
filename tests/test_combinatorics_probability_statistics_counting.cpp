#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/combinatorics_probability_statistics/Counting.hpp"

using namespace aksiomat::combinatorics_probability_statistics;

TEST(Counting, ComputesFactorial) {
	EXPECT_DOUBLE_EQ(Counting::factorial(0), 1);
	EXPECT_DOUBLE_EQ(Counting::factorial(1), 1);
	EXPECT_DOUBLE_EQ(Counting::factorial(5), 120);
	EXPECT_THROW(Counting::factorial(-1), std::invalid_argument);
}

TEST(Counting, ComputesPermutations) {
	EXPECT_DOUBLE_EQ(Counting::permutations(5, 3), 60);
	EXPECT_DOUBLE_EQ(Counting::permutations(5, 0), 1);
	EXPECT_DOUBLE_EQ(Counting::permutations(5, 5), 120);
	EXPECT_THROW(Counting::permutations(3, 5), std::invalid_argument);
	EXPECT_THROW(Counting::permutations(-1, 2), std::invalid_argument);
}

TEST(Counting, ComputesPermutationsWithRepetition) {
	EXPECT_DOUBLE_EQ(Counting::permutationsWithRepetition(3, 2), 9);
	EXPECT_DOUBLE_EQ(Counting::permutationsWithRepetition(0, 0), 1);
	EXPECT_THROW(Counting::permutationsWithRepetition(0, 2), std::invalid_argument);
}

TEST(Counting, ComputesCombinations) {
	EXPECT_DOUBLE_EQ(Counting::combinations(5, 2), 10);
	EXPECT_DOUBLE_EQ(Counting::combinations(6, 0), 1);
	EXPECT_DOUBLE_EQ(Counting::combinations(6, 6), 1);
	EXPECT_THROW(Counting::combinations(2, 5), std::invalid_argument);
}

TEST(Counting, ComputesCombinationsWithRepetition) {
	EXPECT_DOUBLE_EQ(Counting::combinationsWithRepetition(3, 2), 6);
	EXPECT_DOUBLE_EQ(Counting::combinationsWithRepetition(0, 0), 1);
	EXPECT_THROW(Counting::combinationsWithRepetition(0, 2), std::invalid_argument);
}
