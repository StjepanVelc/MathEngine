#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/OrdinaryDifferentialEquations.hpp"

using aksiomat::integral_calculus::OrdinaryDifferentialEquations;

TEST(OdeSolver, SeparableGeneralAndInitial) {
	const auto r = OrdinaryDifferentialEquations::separable("2*x", "y", true, 0, 1, 1, 40);
	EXPECT_TRUE(r.explicitForm);
	EXPECT_TRUE(r.verified) << r.verification;
	EXPECT_NEAR(r.ys.back(), std::exp(1.0), 1e-4);
	EXPECT_FALSE(r.particular.empty());
}

TEST(OdeSolver, SeparableImplicit) {
	const auto r = OrdinaryDifferentialEquations::separable("x", "1/y", true, 0, 1, 1, 40);
	EXPECT_FALSE(r.explicitForm);
	EXPECT_TRUE(r.verified) << r.verification;
	EXPECT_NEAR(r.ys.back(), std::sqrt(2.0), 1e-4);
}

TEST(OdeSolver, SeparableWithoutInitial) {
	const auto r = OrdinaryDifferentialEquations::separable("x", "y", false, 0, 0, 0, 2);
	EXPECT_FALSE(r.general.empty());
	EXPECT_TRUE(r.particular.empty());
}

TEST(OdeSolver, LinearFirstOrder) {
	const auto r = OrdinaryDifferentialEquations::linearFirstOrder("1", "x", true, 0, 0, 2, 40);
	EXPECT_TRUE(r.verified) << r.verification;
	EXPECT_NEAR(r.ys.back(), 2.0 - 1.0 + std::exp(-2.0), 1e-4);
}

TEST(OdeSolver, LinearVariableCoefficient) {
	const auto r = OrdinaryDifferentialEquations::linearFirstOrder("2/x", "x", true, 1, 1, 3, 40);
	EXPECT_TRUE(r.verified) << r.verification;
}

TEST(OdeSolver, GrowthAndLogistic) {
	const auto g = OrdinaryDifferentialEquations::exponentialGrowth(0.1, 1000, 10, 50);
	EXPECT_NEAR(g.ys.back(), 1000 * std::exp(1.0), 1e-2);
	const auto l = OrdinaryDifferentialEquations::logistic(0.5, 100, 10, 20, 100);
	EXPECT_LT(l.ys.back(), 100.0);
	EXPECT_GT(l.ys.back(), 90.0);
}

TEST(OdeSolver, InvalidInput) {
	EXPECT_THROW(OrdinaryDifferentialEquations::separable("x", "y", true, 0, 1, 1, 1), std::invalid_argument);
	EXPECT_THROW(OrdinaryDifferentialEquations::separable("x +* 2", "y", false, 0, 0, 0, 2), std::invalid_argument);
	EXPECT_THROW(OrdinaryDifferentialEquations::separable("1", "exp(y^2)", false, 0, 0, 0, 2), std::invalid_argument);
	EXPECT_THROW(OrdinaryDifferentialEquations::logistic(1, -5, 1, 5, 20), std::invalid_argument);
}
