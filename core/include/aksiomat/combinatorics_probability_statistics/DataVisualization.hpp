#pragma once

#include <string>
#include <vector>

namespace aksiomat::combinatorics_probability_statistics {

struct FrequencyBin {
	double lowerBound;
	double upperBound;
	long long count;
};

struct CategoryFrequency {
	std::string category;
	double value;
};

class DataVisualization {
public:
	static std::vector<FrequencyBin> buildFrequencyTable(const std::vector<double>& dataset, long long binCount);
	static std::vector<CategoryFrequency> buildBarChartData(const std::vector<std::string>& categories, const std::vector<double>& values);
};

} // namespace aksiomat::combinatorics_probability_statistics
