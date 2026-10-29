#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <stdexcept>

#include "aksiomat/integral_calculus/RiemannSums.hpp"

using namespace aksiomat::integral_calculus;

TEST(RiemannSums, LeftAndRightBracketIncreasingFunction) {
	const auto left = RiemannSums::compute("x^2", 0.0, 1.0, 10, RiemannMethod::Left);
	const auto right = RiemannSums::compute("x^2", 0.0, 1.0, 10, RiemannMethod::Right);
	EXPECT_NEAR(left.approximation, 0.285, 1e-12);
	EXPECT_NEAR(right.approximation, 0.385, 1e-12);
	EXPECT_LT(left.approximation, 1.0 / 3.0);
	EXPECT_GT(right.approximation, 1.0 / 3.0);
	EXPECT_EQ(left.rectangles.size(), 10u);
}

TEST(RiemannSums, OtherMethodsAreMoreAccurate) {
	const auto mid = RiemannSums::compute("x^2", 0.0, 1.0, 10, RiemannMethod::Midpoint);
	const auto trap = RiemannSums::compute("x^2", 0.0, 1.0, 10, RiemannMethod::Trapezoid);
	const auto simp = RiemannSums::compute("x^2", 0.0, 1.0, 10, RiemannMethod::Simpson);
	EXPECT_NEAR(mid.approximation, 0.3325, 1e-12);
	EXPECT_NEAR(trap.approximation, 0.335, 1e-12);
	EXPECT_NEAR(simp.approximation, 1.0 / 3.0, 1e-12);
}

TEST(RiemannSums, DarbouxSumsEncloseIntegral) {
	const auto r = RiemannSums::compute("sin(x)", 0.0, std::numbers::pi, 20, RiemannMethod::Midpoint);
	EXPECT_LE(r.lowerSum, 2.0 + 1e-9);
	EXPECT_GE(r.upperSum, 2.0 - 1e-9);
	EXPECT_NEAR(r.reference, 2.0, 1e-9);
	EXPECT_NEAR(r.approximation, 2.0, 0.01);
}

TEST(RiemannSums, NegativeAreaCounts) {
	const auto r = RiemannSums::compute("x", -1.0, 1.0, 100, RiemannMethod::Trapezoid);
	EXPECT_NEAR(r.approximation, 0.0, 1e-12);
}

TEST(RiemannSums, MethodNames) {
	EXPECT_EQ(RiemannSums::parseMethod("simpson"), RiemannMethod::Simpson);
	EXPECT_EQ(RiemannSums::methodId(RiemannMethod::Left), "left");
	EXPECT_THROW(RiemannSums::parseMethod("abc"), std::invalid_argument);
}

TEST(RiemannSums, RejectsInvalidInput) {
	EXPECT_THROW(RiemannSums::compute("x", 1.0, 0.0, 10, RiemannMethod::Left), std::invalid_argument);
	EXPECT_THROW(RiemannSums::compute("x", 0.0, 1.0, 0, RiemannMethod::Left), std::invalid_argument);
	EXPECT_THROW(RiemannSums::compute("x", 0.0, 1.0, 6000, RiemannMethod::Left), std::invalid_argument);
	EXPECT_THROW(RiemannSums::compute("1/x", -1.0, 1.0, 10, RiemannMethod::Left), std::invalid_argument);
	EXPECT_THROW(RiemannSums::compute("x +", 0.0, 1.0, 10, RiemannMethod::Left), std::invalid_argument);
}
