#include "aksiomat/probability_statistics/Distributions.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::probability_statistics;

TEST(Distributions, BinomialComputesKnownProbability) {
	BinomialResult result = Distributions::binomial(10, 0.5, 5);
	EXPECT_NEAR(result.probability, 0.24609375, 1e-6);
	EXPECT_DOUBLE_EQ(result.mean, 5.0);
	EXPECT_DOUBLE_EQ(result.variance, 2.5);
}

TEST(Distributions, BinomialRejectsInvalidProbability) {
	EXPECT_THROW(Distributions::binomial(10, 1.5, 5), std::invalid_argument);
}

TEST(Distributions, PoissonComputesKnownProbability) {
	PoissonResult result = Distributions::poisson(4.0, 2);
	EXPECT_NEAR(result.probability, 0.146525, 1e-5);
	EXPECT_DOUBLE_EQ(result.mean, 4.0);
	EXPECT_DOUBLE_EQ(result.variance, 4.0);
}

TEST(Distributions, PoissonRejectsNonPositiveLambda) {
	EXPECT_THROW(Distributions::poisson(0.0, 2), std::invalid_argument);
}

TEST(Distributions, NormalComputesStandardValues) {
	NormalResult result = Distributions::normal(0.0, 1.0, 0.0);
	EXPECT_NEAR(result.cumulative, 0.5, 1e-9);
	EXPECT_DOUBLE_EQ(result.zScore, 0.0);
	EXPECT_NEAR(result.density, 0.398942, 1e-5);
}

TEST(Distributions, NormalRejectsNonPositiveStdDev) {
	EXPECT_THROW(Distributions::normal(0.0, -1.0, 0.0), std::invalid_argument);
}

TEST(Distributions, UniformComputesMeanAndVariance) {
	UniformResult result = Distributions::uniform(0.0, 10.0, 5.0);
	EXPECT_DOUBLE_EQ(result.mean, 5.0);
	EXPECT_NEAR(result.variance, 8.333333, 1e-5);
	EXPECT_NEAR(result.cumulative, 0.5, 1e-9);
	EXPECT_NEAR(result.density, 0.1, 1e-9);
}

TEST(Distributions, UniformRejectsInvalidBounds) {
	EXPECT_THROW(Distributions::uniform(5.0, 5.0, 3.0), std::invalid_argument);
}
