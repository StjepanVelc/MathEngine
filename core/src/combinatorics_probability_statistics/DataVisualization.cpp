#include "aksiomat/combinatorics_probability_statistics/DataVisualization.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::combinatorics_probability_statistics {
namespace {

void requireNonEmpty(const std::vector<double>& dataset) {
	if (dataset.empty()) throw std::invalid_argument("Skup podataka ne smije biti prazan");
}

} // namespace

std::vector<FrequencyBin> DataVisualization::buildFrequencyTable(const std::vector<double>& dataset, long long binCount) {
	requireNonEmpty(dataset);
	if (binCount <= 0) throw std::invalid_argument("Broj razreda mora biti pozitivan");

	const auto minMax = std::minmax_element(dataset.begin(), dataset.end());
	const double minimum = *minMax.first;
	const double maximum = *minMax.second;
	const double span = (maximum - minimum) <= 0.0 ? 1.0 : (maximum - minimum);
	const double binWidth = span / static_cast<double>(binCount);

	std::vector<FrequencyBin> bins;
	bins.reserve(static_cast<std::size_t>(binCount));
	for (long long index = 0; index < binCount; ++index) {
		const double lowerBound = minimum + binWidth * static_cast<double>(index);
		const double upperBound = (index == binCount - 1) ? maximum : (lowerBound + binWidth);
		bins.push_back({lowerBound, upperBound, 0});
	}

	for (double value : dataset) {
		long long binIndex = static_cast<long long>(std::floor((value - minimum) / binWidth));
		if (binIndex < 0) binIndex = 0;
		if (binIndex >= binCount) binIndex = binCount - 1;
		bins[static_cast<std::size_t>(binIndex)].count += 1;
	}

	return bins;
}

std::vector<CategoryFrequency> DataVisualization::buildBarChartData(const std::vector<std::string>& categories, const std::vector<double>& values) {
	if (categories.empty()) throw std::invalid_argument("Popis kategorija ne smije biti prazan");
	if (categories.size() != values.size()) throw std::invalid_argument("Broj kategorija mora odgovarati broju vrijednosti");

	std::vector<CategoryFrequency> result;
	result.reserve(categories.size());
	for (std::size_t index = 0; index < categories.size(); ++index) {
		if (!std::isfinite(values[index])) throw std::invalid_argument("Vrijednost mora biti konacna");
		result.push_back({categories[index], values[index]});
	}
	return result;
}

} // namespace aksiomat::combinatorics_probability_statistics
