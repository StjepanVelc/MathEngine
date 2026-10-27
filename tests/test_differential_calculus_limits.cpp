#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionContinuity.hpp"
#include "aksiomat/differential_calculus/RealFunctionLimits.hpp"

using namespace aksiomat::differential_calculus;

namespace {
constexpr double kInf = std::numeric_limits<double>::infinity();
}

TEST(RealFunctionLimits, RemovableZeroOverZero) {
	const auto result = RealFunctionLimits::evaluate("sin(x)/x", 0.0);
	EXPECT_EQ(result.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(result.combined.value, 1.0, 1e-6);
	EXPECT_EQ(result.indeterminateForm, "0/0");
	EXPECT_TRUE(result.lHopitalUsed);
	EXPECT_FALSE(result.steps.empty());

	const auto factor = RealFunctionLimits::evaluate("(x^2-1)/(x-1)", 1.0);
	EXPECT_EQ(factor.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(factor.combined.value, 2.0, 1e-6);
	EXPECT_FALSE(factor.valueDefinedAtPoint);
}

TEST(RealFunctionLimits, SecondOrderZeroOverZero) {
	const auto result = RealFunctionLimits::evaluate("(1-cos(x))/x^2", 0.0);
	EXPECT_EQ(result.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(result.combined.value, 0.5, 1e-6);
}

TEST(RealFunctionLimits, OneSidedInfiniteLimits) {
	const auto reciprocal = RealFunctionLimits::evaluate("1/x", 0.0);
	EXPECT_EQ(reciprocal.left.kind, LimitKind::NegativeInfinity);
	EXPECT_EQ(reciprocal.right.kind, LimitKind::PositiveInfinity);
	EXPECT_EQ(reciprocal.combined.kind, LimitKind::DoesNotExist);

	const auto square = RealFunctionLimits::evaluate("1/x^2", 0.0);
	EXPECT_EQ(square.combined.kind, LimitKind::PositiveInfinity);

	const auto logarithm = RealFunctionLimits::evaluate("ln(x)", 0.0);
	EXPECT_EQ(logarithm.left.kind, LimitKind::Undefined);
	EXPECT_EQ(logarithm.right.kind, LimitKind::NegativeInfinity);
}

TEST(RealFunctionLimits, JumpAndOscillation) {
	const auto jump = RealFunctionLimits::evaluate("abs(x)/x", 0.0);
	EXPECT_EQ(jump.left.kind, LimitKind::Finite);
	EXPECT_NEAR(jump.left.value, -1.0, 1e-9);
	EXPECT_NEAR(jump.right.value, 1.0, 1e-9);
	EXPECT_EQ(jump.combined.kind, LimitKind::DoesNotExist);

	const auto oscillation = RealFunctionLimits::evaluate("sin(1/x)", 0.0);
	EXPECT_EQ(oscillation.combined.kind, LimitKind::DoesNotExist);
}

TEST(RealFunctionLimits, DomainEdgeLimit) {
	const auto result = RealFunctionLimits::evaluate("sqrt(x)", 0.0);
	EXPECT_EQ(result.left.kind, LimitKind::Undefined);
	EXPECT_EQ(result.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(result.combined.value, 0.0, 1e-6);
}

TEST(RealFunctionLimits, LimitsAtInfinity) {
	const auto rational = RealFunctionLimits::evaluate("(2x^2+1)/(x^2-3)", kInf);
	EXPECT_TRUE(rational.atInfinity);
	EXPECT_EQ(rational.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(rational.combined.value, 2.0, 1e-6);

	const auto negative = RealFunctionLimits::evaluate("1/x", -kInf);
	EXPECT_EQ(negative.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(negative.combined.value, 0.0, 1e-6);

	const auto growth = RealFunctionLimits::evaluate("exp(x)/x^2", kInf);
	EXPECT_EQ(growth.combined.kind, LimitKind::PositiveInfinity);
	EXPECT_EQ(growth.indeterminateForm, "inf/inf");

	const auto polynomial = RealFunctionLimits::evaluate("x^3", -kInf);
	EXPECT_EQ(polynomial.combined.kind, LimitKind::NegativeInfinity);
}

TEST(RealFunctionLimits, OtherIndeterminateForms) {
	const auto euler = RealFunctionLimits::evaluate("(1+1/x)^x", kInf);
	EXPECT_EQ(euler.indeterminateForm, "1^inf");
	EXPECT_EQ(euler.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(euler.combined.value, std::numbers::e, 1e-3);

	const auto product = RealFunctionLimits::evaluate("x*ln(x)", 0.0);
	EXPECT_EQ(product.indeterminateForm, "0*inf");
	EXPECT_EQ(product.combined.kind, LimitKind::Finite);
	EXPECT_NEAR(product.combined.value, 0.0, 1e-3);

	const auto difference = RealFunctionLimits::evaluate("sqrt(x^2+x)-x", kInf);
	EXPECT_EQ(difference.indeterminateForm, "inf-inf");
	EXPECT_NEAR(difference.combined.value, 0.5, 1e-4);
}

TEST(RealFunctionLimits, ProvidesSamplesForVisualization) {
	const auto result = RealFunctionLimits::evaluate("x^2", 2.0);
	EXPECT_EQ(result.leftSamples.size(), 7u);
	EXPECT_EQ(result.rightSamples.size(), 7u);
	EXPECT_LT(result.leftSamples.front().x, 2.0);
	EXPECT_GT(result.rightSamples.front().x, 2.0);
}

TEST(RealFunctionLimits, RejectsInvalidInput) {
	EXPECT_THROW(RealFunctionLimits::evaluate("x +", 0.0), std::invalid_argument);
	EXPECT_THROW(RealFunctionLimits::evaluate("x", std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

TEST(FunctionContinuity, ClassifiesDiscontinuities) {
	EXPECT_EQ(FunctionContinuity::analyze("x^2", 1.0).kindId, "continuous");
	EXPECT_EQ(FunctionContinuity::analyze("(x^2-1)/(x-1)", 1.0).kindId, "removable");
	EXPECT_EQ(FunctionContinuity::analyze("abs(x)/x", 0.0).kindId, "jump");
	EXPECT_EQ(FunctionContinuity::analyze("1/x", 0.0).kindId, "infinite");
	EXPECT_EQ(FunctionContinuity::analyze("sin(1/x)", 0.0).kindId, "oscillating");
	EXPECT_EQ(FunctionContinuity::analyze("sqrt(x)", 0.0).kindId, "outside_domain");
	EXPECT_EQ(FunctionContinuity::analyze("sin(x)", 0.0).kind, ContinuityKind::Continuous);
}

TEST(FunctionContinuity, RejectsNonFinitePoint) {
	EXPECT_THROW(FunctionContinuity::analyze("x", kInf), std::invalid_argument);
}
