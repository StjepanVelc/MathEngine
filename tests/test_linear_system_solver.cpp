#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/LinearSystemSolver.hpp"

using aksiomat::algebra::LinearSystemSolutionType;
using aksiomat::algebra::LinearSystemSolver;

TEST(LinearSystemSolver, SolvesUniqueTwoByTwoSystem) {
	const auto solution = LinearSystemSolver::solve("x + y = 5", "2x - y = 1");
	EXPECT_EQ(solution.type, LinearSystemSolutionType::Unique);
	EXPECT_DOUBLE_EQ(solution.x, 2.0);
	EXPECT_DOUBLE_EQ(solution.y, 3.0);
	EXPECT_GE(solution.steps.size(), 5u);
	EXPECT_GE(solution.cramerSteps.size(), 5u);
	EXPECT_GE(solution.substitutionSteps.size(), 2u);
	EXPECT_GE(solution.eliminationSteps.size(), 3u);
}

TEST(LinearSystemSolver, SupportsConstantsAndBothSides) {
	const auto solution = LinearSystemSolver::solve("2x + y + 1 = 8", "x - y = -1");
	EXPECT_EQ(solution.type, LinearSystemSolutionType::Unique);
	EXPECT_DOUBLE_EQ(solution.x, 2.0);
	EXPECT_DOUBLE_EQ(solution.y, 3.0);
}

TEST(LinearSystemSolver, ClassifiesDegenerateSystems) {
	EXPECT_EQ(LinearSystemSolver::solve("x + y = 2", "2x + 2y = 4").type,
		LinearSystemSolutionType::Infinite);
	EXPECT_EQ(LinearSystemSolver::solve("x + y = 2", "2x + 2y = 5").type,
		LinearSystemSolutionType::None);
}

TEST(LinearSystemSolver, RejectsNonlinearSystems) {
	EXPECT_THROW(LinearSystemSolver::solve("x*y = 2", "x + y = 3"), std::invalid_argument);
}
