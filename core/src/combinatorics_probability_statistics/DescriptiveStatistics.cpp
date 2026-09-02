#include "aksiomat/combinatorics_probability_statistics/DescriptiveStatistics.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::combinatorics_probability_statistics {
namespace {

void requireNonEmpty(const std::vector<double>& dataset) {
	if (dataset.empty()) throw std::invalid_argument("Skup podataka ne smije biti prazan");
}

double percentile(std::vector<double> sorted, double fraction) {
	const double position = fraction * static_cast<double>(sorted.size() - 1);
	const std::size_t lowerIndex = static_cast<std::size_t>(std::floor(position));
	const std::size_t upperIndex = static_cast<std::size_t>(std::ceil(position));
	if (lowerIndex == upperIndex) return sorted[lowerIndex];
	const double weight = position - static_cast<double>(lowerIndex);
	return sorted[lowerIndex] + weight * (sorted[upperIndex] - sorted[lowerIndex]);
}

} // namespace

double DescriptiveStatistics::mean(const std::vector<double>& dataset) {
	requireNonEmpty(dataset);
	double sum = 0.0;
	for (double value : dataset) sum += value;
	return sum / static_cast<double>(dataset.size());
}

double DescriptiveStatistics::median(const std::vector<double>& dataset) {
	requireNonEmpty(dataset);
	std::vector<double> sorted = dataset;
	std::sort(sorted.begin(), sorted.end());
	const std::size_t size = sorted.size();
	if (size % 2 == 1) return sorted[size / 2];
	return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
}

std::vector<double> DescriptiveStatistics::mode(const std::vector<double>& dataset) {
	requireNonEmpty(dataset);
	std::vector<double> sorted = dataset;
	std::sort(sorted.begin(), sorted.end());

	std::vector<double> uniqueValues;
	std::vector<long long> counts;
	for (double value : sorted) {
		if (!uniqueValues.empty() && uniqueValues.back() == value) {
			++counts.back();
		} else {
			uniqueValues.push_back(value);
			counts.push_back(1);
		}
	}

	const long long maxCount = *std::max_element(counts.begin(), counts.end());
	std::vector<double> modes;
	for (std::size_t index = 0; index < uniqueValues.size(); ++index) {
		if (counts[index] == maxCount) modes.push_back(uniqueValues[index]);
	}
	if (modes.size() == uniqueValues.size()) return {};
	return modes;
}

double DescriptiveStatistics::variance(const std::vector<double>& dataset, bool sampleVariance) {
	requireNonEmpty(dataset);
	if (sampleVariance && dataset.size() < 2) throw std::invalid_argument("Uzoracka varijanca zahtijeva barem dva podatka");
	const double average = mean(dataset);
	double sumSquaredDiff = 0.0;
	for (double value : dataset) {
		const double diff = value - average;
		sumSquaredDiff += diff * diff;
	}
	const double denominator = sampleVariance ? static_cast<double>(dataset.size() - 1) : static_cast<double>(dataset.size());
	return sumSquaredDiff / denominator;
}

double DescriptiveStatistics::standardDeviation(const std::vector<double>& dataset, bool sampleVariance) {
	return std::sqrt(variance(dataset, sampleVariance));
}

double DescriptiveStatistics::range(const std::vector<double>& dataset) {
	requireNonEmpty(dataset);
	const auto minMax = std::minmax_element(dataset.begin(), dataset.end());
	return *minMax.second - *minMax.first;
}

Quartiles DescriptiveStatistics::quartiles(const std::vector<double>& dataset) {
	requireNonEmpty(dataset);
	if (dataset.size() < 2) throw std::invalid_argument("Kvartili zahtijevaju barem dva podatka");
	std::vector<double> sorted = dataset;
	std::sort(sorted.begin(), sorted.end());

	const double q1 = percentile(sorted, 0.25);
	const double q2 = percentile(sorted, 0.5);
	const double q3 = percentile(sorted, 0.75);
	return {q1, q2, q3, q3 - q1};
}

} // namespace aksiomat::combinatorics_probability_statistics
