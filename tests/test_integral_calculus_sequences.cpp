#include <gtest/gtest.h>

#include "aksiomat/integral_calculus/SequenceLimits.hpp"

using aksiomat::integral_calculus::SequenceLimits;

TEST(SequenceLimitsTest, SumIdentityHolds) {
	const auto r = SequenceLimits::verifySumIdentity("n", "n*(n+1)/2", 50);
	EXPECT_TRUE(r.baseCaseHolds);
	EXPECT_TRUE(r.inductionStepVerified);
	EXPECT_EQ(r.firstCounterexample, 0);
	EXPECT_EQ(r.checked, 50);
	const auto s = SequenceLimits::verifySumIdentity("n^2", "n*(n+1)*(2n+1)/6", 50);
	EXPECT_TRUE(s.inductionStepVerified);
}

TEST(SequenceLimitsTest, SumIdentityFails) {
	const auto r = SequenceLimits::verifySumIdentity("n", "n^2", 20);
	EXPECT_TRUE(r.baseCaseHolds);
	EXPECT_FALSE(r.inductionStepVerified);
	EXPECT_EQ(r.firstCounterexample, 2);
}

TEST(SequenceLimitsTest, Inequality) {
	const auto ok = SequenceLimits::verifyInequality("n", "2^n", 1, 40);
	EXPECT_TRUE(ok.inductionStepVerified);
	const auto bad = SequenceLimits::verifyInequality("n^2", "2^n", 1, 40);
	EXPECT_FALSE(bad.inductionStepVerified);
	EXPECT_EQ(bad.firstCounterexample, 3);
}

TEST(SequenceLimitsTest, ConvergentSequence) {
	const auto r = SequenceLimits::analyze("(2n+1)/(n+3)", 0.5, 60);
	EXPECT_TRUE(r.converges);
	EXPECT_NEAR(r.limit, 2.0, 1e-3);
	EXPECT_EQ(r.monotonicity, "rastuci");
	EXPECT_TRUE(r.bounded);
	EXPECT_GT(r.epsilonN, 0);
}

TEST(SequenceLimitsTest, ClassicLimits) {
	EXPECT_NEAR(SequenceLimits::analyze("(1+1/n)^n").limit, 2.718281828, 1e-3);
	EXPECT_NEAR(SequenceLimits::analyze("1/n").limit, 0.0, 1e-9);
}

TEST(SequenceLimitsTest, DivergentSequences) {
	EXPECT_TRUE(SequenceLimits::analyze("n^2").divergesToInfinity);
	const auto osc = SequenceLimits::analyze("(-1)^n");
	EXPECT_FALSE(osc.converges);
	EXPECT_FALSE(osc.divergesToInfinity);
}

TEST(SequenceLimitsTest, InvalidInput) {
	EXPECT_THROW(SequenceLimits::analyze("1/n", -1.0), std::invalid_argument);
	EXPECT_THROW(SequenceLimits::analyze("1/(n-3)"), std::invalid_argument);
	EXPECT_THROW(SequenceLimits::verifySumIdentity("n", "n", 0), std::invalid_argument);
	EXPECT_THROW(SequenceLimits::verifyInequality("n", "n", 5, 2), std::invalid_argument);
}
