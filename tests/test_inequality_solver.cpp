#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/InequalitySolver.hpp"

using aksiomat::algebra::InequalitySolutionType;
using aksiomat::algebra::InequalitySolver;

TEST(InequalitySolver, SolvesUpperAndLowerIntervals) {
	auto solution = InequalitySolver::solve("2x + 3 < 11");
	EXPECT_EQ(solution.type, InequalitySolutionType::Interval);
	EXPECT_EQ(solution.relation, "<");
	EXPECT_DOUBLE_EQ(solution.boundary, 4.0);
	EXPECT_EQ(solution.interval, "(-inf, 4)");

	solution = InequalitySolver::solve("3x - 1 >= 8");
	EXPECT_EQ(solution.relation, ">=");
	EXPECT_DOUBLE_EQ(solution.boundary, 3.0);
}

TEST(InequalitySolver, ReversesRelationForNegativeCoefficient) {
	const auto solution = InequalitySolver::solve("-2x + 1 <= 5");
	EXPECT_EQ(solution.relation, ">=");
	EXPECT_DOUBLE_EQ(solution.boundary, -2.0);
	EXPECT_EQ(solution.interval, "[-2, inf)");
	EXPECT_GE(solution.steps.size(), 4u);
}

TEST(InequalitySolver, ClassifiesConstantInequalities) {
	EXPECT_EQ(InequalitySolver::solve("2x + 1 < 2x + 3").type, InequalitySolutionType::AllReal);
	EXPECT_EQ(InequalitySolver::solve("2x + 4 < 2x + 3").type, InequalitySolutionType::Empty);
}

TEST(InequalitySolver, RejectsInvalidInput) {
	EXPECT_THROW(InequalitySolver::solve("x = 2"), std::invalid_argument);
	EXPECT_THROW(InequalitySolver::solve("x < y"), std::invalid_argument);
	EXPECT_THROW(InequalitySolver::solve("x < 2 < 3"), std::invalid_argument);
}
