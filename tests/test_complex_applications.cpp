#include "aksiomat/complex_numbers/ComplexApplications.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::complex_numbers;

TEST(ComplexApplications, ComputesPowerUsingDeMoivre) {
	PowerResult result = ComplexApplications::power({0, 1}, 2);
	EXPECT_NEAR(result.result.real, -1.0, 1e-9);
	EXPECT_NEAR(result.result.imaginary, 0.0, 1e-9);
}

TEST(ComplexApplications, ComputesFourthRootsOfUnity) {
	RootsResult result = ComplexApplications::nthRoots({1, 0}, 4);
	ASSERT_EQ(result.roots.size(), 4u);
	EXPECT_NEAR(result.roots[0].real, 1.0, 1e-9);
	EXPECT_NEAR(result.roots[0].imaginary, 0.0, 1e-9);
	EXPECT_NEAR(result.roots[1].real, 0.0, 1e-9);
	EXPECT_NEAR(result.roots[1].imaginary, 1.0, 1e-9);
}

TEST(ComplexApplications, NthRootsRejectsNonPositiveDegree) {
	EXPECT_THROW(ComplexApplications::nthRoots({1, 0}, 0), std::invalid_argument);
}

TEST(ComplexApplications, SolvesQuadraticEquationWithRealRoots) {
	QuadraticEquationResult result = ComplexApplications::solveQuadraticEquation(1, -3, 2);
	EXPECT_NEAR(result.root1.real, 2.0, 1e-9);
	EXPECT_NEAR(result.root1.imaginary, 0.0, 1e-9);
	EXPECT_NEAR(result.root2.real, 1.0, 1e-9);
	EXPECT_NEAR(result.root2.imaginary, 0.0, 1e-9);
}

TEST(ComplexApplications, SolvesQuadraticEquationWithComplexRoots) {
	QuadraticEquationResult result = ComplexApplications::solveQuadraticEquation(1, 0, 1);
	EXPECT_NEAR(result.root1.real, 0.0, 1e-9);
	EXPECT_NEAR(result.root1.imaginary, 1.0, 1e-9);
	EXPECT_NEAR(result.root2.real, 0.0, 1e-9);
	EXPECT_NEAR(result.root2.imaginary, -1.0, 1e-9);
}

TEST(ComplexApplications, SolveQuadraticRejectsZeroLeadingCoefficient) {
	EXPECT_THROW(ComplexApplications::solveQuadraticEquation(0, 1, 1), std::invalid_argument);
}
