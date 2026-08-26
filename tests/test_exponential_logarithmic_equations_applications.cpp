#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/exponential_logarithmic/EquationSolvers.hpp"
#include "aksiomat/exponential_logarithmic/Applications.hpp"

using namespace aksiomat::exponential_logarithmic;

TEST(EquationSolvers, SolvesExponentialEquations) {
	const auto solution = EquationSolvers::solveExponential(2, 1, 8);
	EXPECT_EQ(solution.kind, EquationKind::Exponential);
	EXPECT_NEAR(solution.solution, 3, 1e-9);
	EXPECT_THROW(EquationSolvers::solveExponential(1, 1, 8), std::invalid_argument);
	EXPECT_THROW(EquationSolvers::solveExponential(2, 0, 8), std::invalid_argument);
	EXPECT_THROW(EquationSolvers::solveExponential(2, 1, -8), std::invalid_argument);
}

TEST(EquationSolvers, SolvesLogarithmicEquations) {
	const auto solution = EquationSolvers::solveLogarithmic(2, 1, 3);
	EXPECT_EQ(solution.kind, EquationKind::Logarithmic);
	EXPECT_NEAR(solution.solution, 8, 1e-9);
	EXPECT_THROW(EquationSolvers::solveLogarithmic(1, 1, 3), std::invalid_argument);
	EXPECT_THROW(EquationSolvers::solveLogarithmic(2, 0, 3), std::invalid_argument);
}

TEST(Applications, ComputesRealWorldFormulas) {
	EXPECT_NEAR(Applications::radioactiveDecay(100, 10, 10).value, 50, 1e-9);
	EXPECT_NEAR(Applications::phLevel(1e-7).value, 7, 1e-9);
	EXPECT_NEAR(Applications::richterMagnitude(1000).value, 3, 1e-9);
	EXPECT_NEAR(Applications::soundIntensityDecibels(1e-3, 1e-12).value, 90, 1e-9);
	EXPECT_THROW(Applications::radioactiveDecay(-1, 10, 10), std::invalid_argument);
	EXPECT_THROW(Applications::phLevel(0), std::invalid_argument);
}
