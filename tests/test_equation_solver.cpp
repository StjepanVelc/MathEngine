#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/EquationSolver.hpp"

using aksiomat::algebra::EquationSolutionType;
using aksiomat::algebra::EquationSolver;

TEST(EquationSolver, SolvesUniqueLinearEquation) {
	const auto solution = EquationSolver::solve("2x + 3 = 11");
	EXPECT_EQ(solution.type, EquationSolutionType::Unique);
	EXPECT_DOUBLE_EQ(solution.value, 4.0);
	EXPECT_GE(solution.steps.size(), 3u);
}

TEST(EquationSolver, SupportsVariablesOnBothSides) {
	const auto solution = EquationSolver::solve("3(x - 1) = x + 5");
	EXPECT_EQ(solution.type, EquationSolutionType::Unique);
	EXPECT_DOUBLE_EQ(solution.value, 4.0);
}

TEST(EquationSolver, ClassifiesDegenerateEquations) {
	EXPECT_EQ(EquationSolver::solve("2x + 2 = 2(x + 1)").type, EquationSolutionType::Infinite);
	EXPECT_EQ(EquationSolver::solve("2x + 2 = 2x + 3").type, EquationSolutionType::None);
}

TEST(EquationSolver, RejectsNonlinearOrMalformedEquations) {
	EXPECT_THROW(EquationSolver::solve("x^2 = 4"), std::invalid_argument);
	EXPECT_THROW(EquationSolver::solve("2x + 1"), std::invalid_argument);
	EXPECT_THROW(EquationSolver::solve("x = y"), std::invalid_argument);
}
