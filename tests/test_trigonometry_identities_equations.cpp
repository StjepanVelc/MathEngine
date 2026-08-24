#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "aksiomat/trigonometry/IdentitiesEquations.hpp"

using namespace aksiomat::trigonometry;

TEST(TrigonometryIdentities, ChecksFundamentalAndDoubleAngleIdentities) {
	for (const auto identity : {TrigIdentity::Pythagorean, TrigIdentity::TangentRatio,
		TrigIdentity::DoubleAngleSine, TrigIdentity::DoubleAngleCosine}) {
		const auto result = IdentitiesEquations::check(identity, 37.0);
		EXPECT_TRUE(result.holds);
		EXPECT_NEAR(result.left, result.right, 1e-10);
		EXPECT_FALSE(result.formula.empty());
	}
}

TEST(TrigonometryIdentities, RejectsUndefinedTangentIdentity) {
	EXPECT_THROW(IdentitiesEquations::check(TrigIdentity::TangentRatio, 90.0), std::invalid_argument);
	EXPECT_THROW(IdentitiesEquations::check(static_cast<TrigIdentity>(99), 30.0), std::invalid_argument);
}

TEST(TrigonometryEquations, SolvesSineOnStandardInterval) {
	const auto solutions = IdentitiesEquations::solve(TrigFunction::Sine, 0.5, 0.0, 360.0);
	ASSERT_EQ(solutions.size(), 2u);
	EXPECT_NEAR(solutions[0], 30.0, 1e-10);
	EXPECT_NEAR(solutions[1], 150.0, 1e-10);
}

TEST(TrigonometryEquations, SolvesCosineAndTangentOnIntervals) {
	const auto cosine = IdentitiesEquations::solve(TrigFunction::Cosine, 0.0, -180.0, 180.0);
	EXPECT_EQ(cosine, (std::vector<double>{-90.0, 90.0}));
	const auto tangent = IdentitiesEquations::solve(TrigFunction::Tangent, 1.0, 0.0, 360.0);
	ASSERT_EQ(tangent.size(), 2u);
	EXPECT_NEAR(tangent[0], 45.0, 1e-10);
	EXPECT_NEAR(tangent[1], 225.0, 1e-10);
}

TEST(TrigonometryEquations, HandlesNoSolutionAndValidatesInterval) {
	EXPECT_TRUE(IdentitiesEquations::solve(TrigFunction::Sine, 2.0, 0.0, 360.0).empty());
	EXPECT_THROW(IdentitiesEquations::solve(TrigFunction::Cosine, 0.5, 10.0, -10.0), std::invalid_argument);
	EXPECT_THROW(IdentitiesEquations::solve(static_cast<TrigFunction>(99), 0.0, 0.0, 360.0), std::invalid_argument);
}
