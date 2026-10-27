#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/DerivativeAtPoint.hpp"
#include "aksiomat/differential_calculus/FunctionStudy.hpp"

using namespace aksiomat::differential_calculus;

TEST(DerivativeAtPoint, TangentAndNormalOfParabola) {
	const auto result = DerivativeAtPoint::analyze("x^2", 3.0);
	EXPECT_TRUE(result.differentiable);
	EXPECT_NEAR(result.slope, 6.0, 1e-6);
	EXPECT_EQ(result.tangentLine, "y = 6*x - 9");
	EXPECT_EQ(result.derivativeExpression, "2*x");
	EXPECT_EQ(result.rightRows.size(), 6u);
	EXPECT_NEAR(result.rightRows.back().secantSlope, 6.0001, 1e-6);
	EXPECT_NEAR(result.leftRows.back().secantSlope, 5.9999, 1e-6);
}

TEST(DerivativeAtPoint, HorizontalTangent) {
	const auto result = DerivativeAtPoint::analyze("x^2 - 2x", 1.0);
	EXPECT_TRUE(result.differentiable);
	EXPECT_NEAR(result.slope, 0.0, 1e-6);
	EXPECT_EQ(result.tangentLine, "y = -1");
	EXPECT_EQ(result.normalLine, "x = 1");
}

TEST(DerivativeAtPoint, KinkIsNotDifferentiable) {
	const auto result = DerivativeAtPoint::analyze("abs(x)", 0.0);
	EXPECT_FALSE(result.differentiable);
	EXPECT_EQ(result.problem, "kink");
	EXPECT_NEAR(result.leftDerivative, -1.0, 1e-6);
	EXPECT_NEAR(result.rightDerivative, 1.0, 1e-6);
}

TEST(DerivativeAtPoint, UndefinedAndOneSided) {
	EXPECT_EQ(DerivativeAtPoint::analyze("ln(x)", -1.0).problem, "undefined");
	EXPECT_EQ(DerivativeAtPoint::analyze("sqrt(x)", 0.0).problem, "one_sided");
}

TEST(DerivativeAtPoint, CubicHasHorizontalTangentAtZero) {
	const auto result = DerivativeAtPoint::analyze("x^3", 0.0);
	EXPECT_TRUE(result.differentiable);
	EXPECT_NEAR(result.slope, 0.0, 1e-6);
}

TEST(MeanValueTheorems, LagrangeOnParabola) {
	const auto result = DerivativeAtPoint::meanValue("x^2", 0.0, 2.0);
	EXPECT_TRUE(result.lagrangeApplies);
	EXPECT_FALSE(result.rolleApplies);
	EXPECT_NEAR(result.meanSlope, 2.0, 1e-12);
	ASSERT_EQ(result.points.size(), 1u);
	EXPECT_NEAR(result.points[0], 1.0, 1e-6);
}

TEST(MeanValueTheorems, RolleOnParabola) {
	const auto result = DerivativeAtPoint::meanValue("x^2 - 2x", 0.0, 2.0);
	EXPECT_TRUE(result.rolleApplies);
	ASSERT_EQ(result.points.size(), 1u);
	EXPECT_NEAR(result.points[0], 1.0, 1e-6);
}

TEST(MeanValueTheorems, ConditionsFail) {
	EXPECT_FALSE(DerivativeAtPoint::meanValue("1/x", -1.0, 1.0).lagrangeApplies);
	const auto kink = DerivativeAtPoint::meanValue("abs(x)", -1.0, 1.0);
	EXPECT_FALSE(kink.differentiableInInterior);
	EXPECT_TRUE(kink.points.empty());
	EXPECT_THROW(DerivativeAtPoint::meanValue("x", 2.0, 1.0), std::invalid_argument);
}

TEST(FunctionStudy, CubicExtremaZerosAndInflection) {
	const auto result = FunctionStudy::analyze("x^3 - 3x", -3.0, 3.0);
	EXPECT_EQ(result.firstDerivative, "3*x^2 - 3");
	ASSERT_EQ(result.zeros.size(), 3u);
	EXPECT_NEAR(result.zeros[0], -std::sqrt(3.0), 1e-6);
	EXPECT_NEAR(result.zeros[1], 0.0, 1e-6);
	ASSERT_EQ(result.criticalPoints.size(), 2u);
	EXPECT_EQ(result.criticalPoints[0].kind, "maximum");
	EXPECT_NEAR(result.criticalPoints[0].x, -1.0, 1e-6);
	EXPECT_NEAR(result.criticalPoints[0].y, 2.0, 1e-6);
	EXPECT_EQ(result.criticalPoints[1].kind, "minimum");
	ASSERT_EQ(result.inflectionPoints.size(), 1u);
	EXPECT_NEAR(result.inflectionPoints[0].x, 0.0, 1e-6);
	ASSERT_EQ(result.monotonicity.size(), 3u);
	EXPECT_EQ(result.monotonicity[0].behavior, "increasing");
	EXPECT_EQ(result.monotonicity[1].behavior, "decreasing");
	ASSERT_EQ(result.concavity.size(), 2u);
	EXPECT_EQ(result.concavity[0].behavior, "concave");
	EXPECT_EQ(result.concavity[1].behavior, "convex");
	EXPECT_EQ(result.f.size(), 401u);
}

TEST(FunctionStudy, StationaryPointWithoutExtremum) {
	const auto result = FunctionStudy::analyze("x^3", -2.0, 2.0);
	ASSERT_EQ(result.criticalPoints.size(), 1u);
	EXPECT_EQ(result.criticalPoints[0].kind, "stationary");
}

TEST(FunctionStudy, ParabolaMinimum) {
	const auto result = FunctionStudy::analyze("x^2 - 4x + 3", -1.0, 5.0);
	ASSERT_EQ(result.criticalPoints.size(), 1u);
	EXPECT_EQ(result.criticalPoints[0].kind, "minimum");
	EXPECT_NEAR(result.criticalPoints[0].x, 2.0, 1e-6);
	EXPECT_NEAR(result.criticalPoints[0].y, -1.0, 1e-6);
	EXPECT_EQ(result.zeros.size(), 2u);
}

TEST(FunctionStudy, FindsAsymptotes) {
	const auto reciprocal = FunctionStudy::analyze("1/x", -3.0, 3.0);
	bool vertical = false;
	bool horizontal = false;
	for (const auto& a : reciprocal.asymptotes) {
		if (a.kind == "vertical" && std::fabs(a.x) < 1e-6) vertical = true;
		if (a.kind == "horizontal" && a.equation == "y = 0") horizontal = true;
	}
	EXPECT_TRUE(vertical);
	EXPECT_TRUE(horizontal);

	const auto slant = FunctionStudy::analyze("(x^2+1)/x", -5.0, 5.0);
	bool hasSlant = false;
	for (const auto& a : slant.asymptotes) {
		if (a.kind == "slant" && a.equation == "y = x") hasSlant = true;
	}
	EXPECT_TRUE(hasSlant);
}

TEST(FunctionStudy, DomainRestrictions) {
	const auto result = FunctionStudy::analyze("sqrt(x)", -2.0, 4.0);
	ASSERT_EQ(result.domain.size(), 1u);
	EXPECT_NEAR(result.domain[0].from, 0.0, 1e-6);
	EXPECT_NEAR(result.domain[0].to, 4.0, 1e-9);
}

TEST(FunctionStudy, SamplingAndValidation) {
	const auto points = FunctionStudy::sample("ln(x)", -1.0, 1.0, 10);
	ASSERT_EQ(points.size(), 11u);
	EXPECT_FALSE(points.front().defined);
	EXPECT_TRUE(points.back().defined);
	EXPECT_THROW(FunctionStudy::sample("x", 1.0, 1.0, 10), std::invalid_argument);
	EXPECT_THROW(FunctionStudy::analyze("x", 0.0, 1.0, 5), std::invalid_argument);
}
