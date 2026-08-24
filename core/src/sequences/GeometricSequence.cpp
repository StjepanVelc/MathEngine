#include "aksiomat/sequences/GeometricSequence.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::sequences {
namespace {

constexpr std::size_t maximumGeneratedTerms = 10000;
constexpr double equalityTolerance = 1e-12;

void requireFinite(double value, const char* message) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(message);
	}
}

void requireIndex(std::size_t index) {
	if (index == 0) {
		throw std::invalid_argument("Sequence index starts at 1");
	}
}

void requireTermCount(std::size_t termCount) {
	if (termCount == 0 || termCount > maximumGeneratedTerms) {
		throw std::invalid_argument("Term count must be between 1 and 10000");
	}
}

} // namespace

GeometricSequence::GeometricSequence(double firstTerm, double ratio)
	: firstTerm_(firstTerm), ratio_(ratio) {
	requireFinite(firstTerm, "First term must be finite");
	requireFinite(ratio, "Ratio must be finite");
}

double GeometricSequence::firstTerm() const noexcept {
	return firstTerm_;
}

double GeometricSequence::ratio() const noexcept {
	return ratio_;
}

double GeometricSequence::term(std::size_t index) const {
	requireIndex(index);
	const double result = firstTerm_ * std::pow(ratio_, static_cast<double>(index - 1));
	requireFinite(result, "Geometric sequence term overflowed");
	return result;
}

double GeometricSequence::partialSum(std::size_t termCount) const {
	requireTermCount(termCount);
	if (std::abs(ratio_ - 1.0) <= equalityTolerance) {
		const double result = firstTerm_ * static_cast<double>(termCount);
		requireFinite(result, "Geometric partial sum overflowed");
		return result;
	}

	const double result = firstTerm_ * (1.0 - std::pow(ratio_, static_cast<double>(termCount))) / (1.0 - ratio_);
	requireFinite(result, "Geometric partial sum overflowed");
	return result;
}

std::optional<std::size_t> GeometricSequence::indexOf(double value, std::size_t maxIndex, double tolerance) const {
	requireFinite(value, "Searched value must be finite");
	if (maxIndex == 0 || maxIndex > maximumGeneratedTerms) {
		throw std::invalid_argument("Maximum index must be between 1 and 10000");
	}
	if (!std::isfinite(tolerance) || tolerance < 0.0) {
		throw std::invalid_argument("Tolerance must be finite and non-negative");
	}

	for (std::size_t index = 1; index <= maxIndex; ++index) {
		double current;
		try {
			current = term(index);
		} catch (const std::invalid_argument&) {
			return std::nullopt;
		}
		if (std::abs(current - value) <= tolerance * std::max(1.0, std::abs(value))) {
			return index;
		}
	}
	return std::nullopt;
}

InfiniteSeriesResult GeometricSequence::infiniteSum() const {
	if (std::abs(ratio_) >= 1.0) {
		return {false, std::nullopt};
	}
	const double result = firstTerm_ / (1.0 - ratio_);
	requireFinite(result, "Infinite geometric series sum overflowed");
	return {true, result};
}

std::vector<double> GeometricSequence::generate(std::size_t termCount) const {
	requireTermCount(termCount);
	std::vector<double> terms;
	terms.reserve(termCount);
	for (std::size_t index = 1; index <= termCount; ++index) {
		terms.push_back(term(index));
	}
	return terms;
}

} // namespace aksiomat::sequences
