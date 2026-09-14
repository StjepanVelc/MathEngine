#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/mathematical_analysis/DifferentialEquations.hpp"

using aksiomat::mathematical_analysis::DifferentialEquations;

TEST(DifferentialEquations, EulerApproximatesExponentialGrowth) {
	// y' = y, y(0) = 1 => y(t) = e^t; Euler with small step should approximate e^1 roughly
	const auto f = [](double, double y) { return y; };
	const auto result = DifferentialEquations::solveEuler(f, "y' = y", 0.0, 1.0, 1.0, 0.01);
	EXPECT_NEAR(result.finalValue, std::exp(1.0), 0.02);
	EXPECT_EQ(result.method, "Euler");
	EXPECT_FALSE(result.points.empty());
	EXPECT_FALSE(result.steps.empty());
}

TEST(DifferentialEquations, RungeKutta4IsMoreAccurateThanEuler) {
	const auto f = [](double, double y) { return y; };
	const auto euler = DifferentialEquations::solveEuler(f, "y' = y", 0.0, 1.0, 1.0, 0.1);
	const auto rk4 = DifferentialEquations::solveRungeKutta4(f, "y' = y", 0.0, 1.0, 1.0, 0.1);
	const double eulerError = std::abs(euler.finalValue - std::exp(1.0));
	const double rk4Error = std::abs(rk4.finalValue - std::exp(1.0));
	EXPECT_LT(rk4Error, eulerError);
	EXPECT_NEAR(rk4.finalValue, std::exp(1.0), 1e-4);
}

TEST(DifferentialEquations, RejectsNonPositiveStepSize) {
	const auto f = [](double, double y) { return y; };
	EXPECT_THROW(DifferentialEquations::solveEuler(f, "y' = y", 0.0, 1.0, 1.0, 0.0), std::invalid_argument);
}

TEST(DifferentialEquations, RejectsFinalTBeforeInitialT) {
	const auto f = [](double, double y) { return y; };
	EXPECT_THROW(DifferentialEquations::solveRungeKutta4(f, "y' = y", 1.0, 1.0, 0.0, 0.1), std::invalid_argument);
}
