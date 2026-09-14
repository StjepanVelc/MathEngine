#include "aksiomat/discrete_math/Recurrences.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::discrete_math;

TEST(Recurrences, SolvesDistinctRealRoots) {
	// a_n = a_(n-1) + 2*a_(n-2), a0=0, a1=1 -> roots 2 and -1.
	RecurrenceSolveResult result = Recurrences::solveLinearSecondOrder(1, 2, 0, 1);
	EXPECT_FALSE(result.repeatedRoot);
	EXPECT_NEAR(std::max(result.root1, result.root2), 2.0, 1e-9);
	EXPECT_NEAR(std::min(result.root1, result.root2), -1.0, 1e-9);
}

TEST(Recurrences, SolvesRepeatedRoot) {
	// a_n = 2*a_(n-1) - a_(n-2) -> root x=1 (double), a0=1, a1=2.
	RecurrenceSolveResult result = Recurrences::solveLinearSecondOrder(2, -1, 1, 2);
	EXPECT_TRUE(result.repeatedRoot);
	EXPECT_NEAR(result.root1, 1.0, 1e-9);
}

TEST(Recurrences, ThrowsOnComplexRoots) {
	EXPECT_THROW(Recurrences::solveLinearSecondOrder(0, -1, 0, 1), std::invalid_argument);
}

TEST(Recurrences, GeneratesFibonacciTerms) {
	RecurrenceTermsResult result = Recurrences::generateTerms(1, 1, 0, 1, 8);
	std::vector<double> expected{0, 1, 1, 2, 3, 5, 8, 13};
	ASSERT_EQ(result.terms.size(), expected.size());
	for (std::size_t i = 0; i < expected.size(); ++i) {
		EXPECT_NEAR(result.terms[i], expected[i], 1e-9);
	}
}

TEST(Recurrences, ThrowsOnNegativeCount) {
	EXPECT_THROW(Recurrences::generateTerms(1, 1, 0, 1, -1), std::invalid_argument);
}
