#pragma once

#include <vector>

namespace aksiomat::combinatorics_probability_statistics {

struct Quartiles {
	double q1;
	double q2;
	double q3;
	double interquartileRange;
};

class DescriptiveStatistics {
public:
	static double mean(const std::vector<double>& dataset);
	static double median(const std::vector<double>& dataset);
	static std::vector<double> mode(const std::vector<double>& dataset);
	static double variance(const std::vector<double>& dataset, bool sampleVariance);
	static double standardDeviation(const std::vector<double>& dataset, bool sampleVariance);
	static double range(const std::vector<double>& dataset);
	static Quartiles quartiles(const std::vector<double>& dataset);
};

} // namespace aksiomat::combinatorics_probability_statistics
