#include "aksiomat/sequences/SequenceAnalysis.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::sequences {
namespace {

constexpr double tolerance = 1e-12;

void requireFinite(double value) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument("Sequence terms must be finite");
	}
}

} // namespace

SequenceSummary SequenceAnalysis::analyze(const std::vector<double>& terms) {
	if (terms.empty()) {
		throw std::invalid_argument("Sequence must contain at least one term");
	}

	SequenceSummary result;
	result.minimum = terms.front();
	result.maximum = terms.front();
	result.points.reserve(terms.size());
	if (terms.size() > 1) {
		result.differences.reserve(terms.size() - 1);
	}

	bool hasPositive = false;
	bool hasNegative = false;
	bool hasZero = false;
	for (std::size_t i = 0; i < terms.size(); ++i) {
		requireFinite(terms[i]);
		result.points.push_back({i + 1, terms[i]});
		result.minimum = std::min(result.minimum, terms[i]);
		result.maximum = std::max(result.maximum, terms[i]);
		if (i == 0) {
			continue;
		}

		const double difference = terms[i] - terms[i - 1];
		result.differences.push_back(difference);
		hasPositive = hasPositive || difference > tolerance;
		hasNegative = hasNegative || difference < -tolerance;
		hasZero = hasZero || std::abs(difference) <= tolerance;
	}

	if (!hasPositive && !hasNegative) {
		result.monotonicity = Monotonicity::Constant;
	} else if (hasPositive && !hasNegative) {
		result.monotonicity = hasZero ? Monotonicity::NonDecreasing : Monotonicity::StrictlyIncreasing;
	} else if (hasNegative && !hasPositive) {
		result.monotonicity = hasZero ? Monotonicity::NonIncreasing : Monotonicity::StrictlyDecreasing;
	} else {
		result.monotonicity = Monotonicity::NotMonotonic;
	}
	return result;
}

} // namespace aksiomat::sequences
