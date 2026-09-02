#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "aksiomat/combinatorics_probability_statistics/DescriptiveStatistics.hpp"

using namespace aksiomat::combinatorics_probability_statistics;

TEST(DescriptiveStatistics, ComputesMean) {
	EXPECT_DOUBLE_EQ(DescriptiveStatistics::mean({1, 2, 3, 4, 5}), 3);
	EXPECT_THROW(DescriptiveStatistics::mean({}), std::invalid_argument);
}

TEST(DescriptiveStatistics, ComputesMedian) {
	EXPECT_DOUBLE_EQ(DescriptiveStatistics::median({1, 3, 2}), 2);
	EXPECT_DOUBLE_EQ(DescriptiveStatistics::median({1, 2, 3, 4}), 2.5);
	EXPECT_THROW(DescriptiveStatistics::median({}), std::invalid_argument);
}

TEST(DescriptiveStatistics, ComputesMode) {
	const auto modes = DescriptiveStatistics::mode({1, 2, 2, 3});
	ASSERT_EQ(modes.size(), 1u);
	EXPECT_DOUBLE_EQ(modes[0], 2);
	EXPECT_TRUE(DescriptiveStatistics::mode({1, 2, 3}).empty());
	EXPECT_THROW(DescriptiveStatistics::mode({}), std::invalid_argument);
}

TEST(DescriptiveStatistics, ComputesVarianceAndStandardDeviation) {
	const std::vector<double> dataset = {2, 4, 4, 4, 5, 5, 7, 9};
	EXPECT_NEAR(DescriptiveStatistics::variance(dataset, false), 4.0, 1e-9);
	EXPECT_NEAR(DescriptiveStatistics::standardDeviation(dataset, false), 2.0, 1e-9);
	EXPECT_THROW(DescriptiveStatistics::variance({1}, true), std::invalid_argument);
}

TEST(DescriptiveStatistics, ComputesRange) {
	EXPECT_DOUBLE_EQ(DescriptiveStatistics::range({1, 5, 3, 9, 2}), 8);
	EXPECT_THROW(DescriptiveStatistics::range({}), std::invalid_argument);
}

TEST(DescriptiveStatistics, ComputesQuartiles) {
	const auto result = DescriptiveStatistics::quartiles({1, 2, 3, 4, 5, 6, 7, 8});
	EXPECT_NEAR(result.q2, 4.5, 1e-9);
	EXPECT_GT(result.q3, result.q1);
	EXPECT_NEAR(result.interquartileRange, result.q3 - result.q1, 1e-9);
	EXPECT_THROW(DescriptiveStatistics::quartiles({1}), std::invalid_argument);
}
