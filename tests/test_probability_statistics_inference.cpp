#include "aksiomat/probability_statistics/StatisticalInference.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::probability_statistics;

TEST(StatisticalInference, ConfidenceIntervalComputesExpectedBounds) {
	std::vector<double> sample{10, 12, 11, 13, 9, 14, 10, 12};
	ConfidenceIntervalResult result = StatisticalInference::confidenceIntervalForMean(sample, 0.95, 2.0);
	EXPECT_NEAR(result.sampleMean, 11.375, 1e-9);
	EXPECT_GT(result.marginOfError, 0.0);
	EXPECT_LT(result.lowerBound, result.sampleMean);
	EXPECT_GT(result.upperBound, result.sampleMean);
}

TEST(StatisticalInference, ConfidenceIntervalRejectsInvalidLevel) {
	std::vector<double> sample{1, 2, 3};
	EXPECT_THROW(StatisticalInference::confidenceIntervalForMean(sample, 1.5, 1.0), std::invalid_argument);
}

TEST(StatisticalInference, ZTestRejectsNullWhenFarFromHypothesizedMean) {
	std::vector<double> sample{20, 21, 19, 22, 20, 21, 19, 20};
	HypothesisTestResult result = StatisticalInference::zTestForMean(sample, 10.0, 1.5, 0.05);
	EXPECT_TRUE(result.rejectNull);
}

TEST(StatisticalInference, ZTestDoesNotRejectNullWhenCloseToHypothesizedMean) {
	std::vector<double> sample{10.1, 9.9, 10.0, 10.2, 9.8};
	HypothesisTestResult result = StatisticalInference::zTestForMean(sample, 10.0, 1.0, 0.05);
	EXPECT_FALSE(result.rejectNull);
}

TEST(StatisticalInference, SimpleLinearRegressionFindsPerfectFit) {
	std::vector<double> x{1, 2, 3, 4, 5};
	std::vector<double> y{2, 4, 6, 8, 10};
	RegressionResult result = StatisticalInference::simpleLinearRegression(x, y);
	EXPECT_NEAR(result.slope, 2.0, 1e-9);
	EXPECT_NEAR(result.intercept, 0.0, 1e-9);
	EXPECT_NEAR(result.correlation, 1.0, 1e-9);
	EXPECT_NEAR(result.rSquared, 1.0, 1e-9);
}

TEST(StatisticalInference, SimpleLinearRegressionRejectsMismatchedLengths) {
	std::vector<double> x{1, 2, 3};
	std::vector<double> y{1, 2};
	EXPECT_THROW(StatisticalInference::simpleLinearRegression(x, y), std::invalid_argument);
}
