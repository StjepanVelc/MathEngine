#include <gtest/gtest.h>

#include <cmath>

#include "aksiomat/integral_calculus/DefiniteIntegral.hpp"

using namespace aksiomat::integral_calculus;

TEST(DefiniteIntegral, NewtonLeibniz) {
	const auto r = DefiniteIntegration::integrate("x^2", 0, 3);
	EXPECT_TRUE(r.symbolic);
	EXPECT_NEAR(r.value, 9.0, 1e-9);
	EXPECT_TRUE(r.agrees);
	EXPECT_EQ(r.method, "Newton-Leibniz");
}

TEST(DefiniteIntegral, SwappedBoundsChangeSign) {
	EXPECT_NEAR(DefiniteIntegration::integrate("x", 2, 0).value, -2.0, 1e-9);
	EXPECT_EQ(DefiniteIntegration::integrate("x", 1, 1).value, 0.0);
}

TEST(DefiniteIntegral, TrigAndParts) {
	EXPECT_NEAR(DefiniteIntegration::integrate("sin(x)", 0, 3.14159265358979).value, 2.0, 1e-6);
	EXPECT_NEAR(DefiniteIntegration::integrate("x*exp(x)", 0, 1).value, 1.0, 1e-9);
}

TEST(DefiniteIntegral, FallsBackToNumeric) {
	const auto r = DefiniteIntegration::integrate("exp(x^2)", 0, 1);
	EXPECT_FALSE(r.symbolic);
	EXPECT_NEAR(r.value, 1.4626517459, 1e-6);
}

TEST(DefiniteIntegral, SingularityIsRejected) {
	EXPECT_THROW(DefiniteIntegration::integrate("1/x", -1, 1), std::exception);
	EXPECT_THROW(DefiniteIntegration::integrate("x", INFINITY, 1), std::invalid_argument);
}

TEST(DefiniteIntegral, AreaBetweenCurves) {
	const auto r = DefiniteIntegration::areaBetween("x", "x^2", 0, 1);
	EXPECT_NEAR(r.area, 1.0 / 6.0, 1e-9);
	const auto s = DefiniteIntegration::areaBetween("sin(x)", "0", 0, 6.28318530718);
	EXPECT_NEAR(s.area, 4.0, 1e-5);
	EXPECT_FALSE(s.crossings.empty());
	EXPECT_THROW(DefiniteIntegration::areaBetween("x", "x^2", 1, 0), std::invalid_argument);
}

TEST(DefiniteIntegral, ImproperIntegral) {
	const auto conv = DefiniteIntegration::improper("1/x^2", 1, false, 0, true);
	EXPECT_EQ(conv.kind, ImproperKind::Convergent);
	EXPECT_NEAR(conv.value, 1.0, 1e-3);
	const auto div = DefiniteIntegration::improper("1/x", 1, false, 0, true);
	EXPECT_EQ(div.kind, ImproperKind::Divergent);
	const auto gauss = DefiniteIntegration::improper("exp(-x^2)", 0, true, 0, true);
	EXPECT_EQ(gauss.kind, ImproperKind::Convergent);
	EXPECT_NEAR(gauss.value, 1.7724538509, 1e-3);
	EXPECT_THROW(DefiniteIntegration::improper("x", 0, false, 1, false), std::invalid_argument);
}

TEST(DefiniteIntegral, SingularEndpoint) {
	const auto r = DefiniteIntegration::improperSingular("1/sqrt(x)", 0, 1, true);
	EXPECT_EQ(r.kind, ImproperKind::Convergent);
	EXPECT_NEAR(r.value, 2.0, 1e-2);
	EXPECT_EQ(DefiniteIntegration::improperSingular("1/x", 0, 1, true).kind, ImproperKind::Divergent);
	EXPECT_THROW(DefiniteIntegration::improperSingular("1/x", 1, 0, true), std::invalid_argument);
}

TEST(DefiniteIntegral, PTest) {
	EXPECT_EQ(DefiniteIntegration::pTest(2, true).kind, ImproperKind::Convergent);
	EXPECT_NEAR(DefiniteIntegration::pTest(3, true).value, 0.5, 1e-12);
	EXPECT_EQ(DefiniteIntegration::pTest(1, true).kind, ImproperKind::Divergent);
	EXPECT_EQ(DefiniteIntegration::pTest(0.5, false).kind, ImproperKind::Convergent);
	EXPECT_NEAR(DefiniteIntegration::pTest(0.5, false).value, 2.0, 1e-12);
	EXPECT_EQ(DefiniteIntegration::pTest(2, false).kind, ImproperKind::Divergent);
}

TEST(DefiniteIntegral, MeanValue) {
	const auto r = DefiniteIntegration::meanValue("x^2", 0, 3);
	EXPECT_NEAR(r.value / 3.0, 3.0, 1e-9);
	EXPECT_FALSE(r.steps.empty());
}
