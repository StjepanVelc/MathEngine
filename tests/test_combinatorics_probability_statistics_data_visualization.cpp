#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "aksiomat/combinatorics_probability_statistics/DataVisualization.hpp"

using namespace aksiomat::combinatorics_probability_statistics;

TEST(DataVisualization, BuildsFrequencyTable) {
	const std::vector<double> dataset = {1, 2, 2, 3, 4, 5, 5, 5, 9};
	const auto bins = DataVisualization::buildFrequencyTable(dataset, 4);
	ASSERT_EQ(bins.size(), 4u);

	long long totalCount = 0;
	for (const auto& bin : bins) totalCount += bin.count;
	EXPECT_EQ(totalCount, static_cast<long long>(dataset.size()));

	EXPECT_THROW(DataVisualization::buildFrequencyTable({}, 4), std::invalid_argument);
	EXPECT_THROW(DataVisualization::buildFrequencyTable(dataset, 0), std::invalid_argument);
}

TEST(DataVisualization, BuildsBarChartData) {
	const std::vector<std::string> categories = {"A", "B", "C"};
	const std::vector<double> values = {3, 7, 2};
	const auto result = DataVisualization::buildBarChartData(categories, values);
	ASSERT_EQ(result.size(), 3u);
	EXPECT_EQ(result[1].category, "B");
	EXPECT_DOUBLE_EQ(result[1].value, 7);

	EXPECT_THROW(DataVisualization::buildBarChartData({}, {}), std::invalid_argument);
	EXPECT_THROW(DataVisualization::buildBarChartData(categories, {1, 2}), std::invalid_argument);
}
