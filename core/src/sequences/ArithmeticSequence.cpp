#include "aksiomat/sequences/ArithmeticSequence.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace aksiomat::sequences {
namespace {

constexpr std::size_t maximumGeneratedTerms = 10000;

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

ArithmeticSequence::ArithmeticSequence(double firstTerm, double difference)
	: firstTerm_(firstTerm), difference_(difference) {
	requireFinite(firstTerm, "First term must be finite");
	requireFinite(difference, "Difference must be finite");
}

double ArithmeticSequence::firstTerm() const noexcept {
	return firstTerm_;
}

double ArithmeticSequence::difference() const noexcept {
	return difference_;
}

double ArithmeticSequence::term(std::size_t index) const {
	requireIndex(index);
	const double result = firstTerm_ + static_cast<double>(index - 1) * difference_;
	requireFinite(result, "Arithmetic sequence term overflowed");
	return result;
}

double ArithmeticSequence::partialSum(std::size_t termCount) const {
	requireTermCount(termCount);
	const double count = static_cast<double>(termCount);
	const double result = count * (2.0 * firstTerm_ + (count - 1.0) * difference_) / 2.0;
	requireFinite(result, "Arithmetic partial sum overflowed");
	return result;
}

std::optional<std::size_t> ArithmeticSequence::indexOf(double value, double tolerance) const {
	requireFinite(value, "Searched value must be finite");
	if (!std::isfinite(tolerance) || tolerance < 0.0) {
		throw std::invalid_argument("Tolerance must be finite and non-negative");
	}
	if (std::abs(difference_) <= std::numeric_limits<double>::epsilon()) {
		return std::abs(value - firstTerm_) <= tolerance ? std::optional<std::size_t>{1} : std::nullopt;
	}

	const double rawIndex = (value - firstTerm_) / difference_ + 1.0;
	if (rawIndex < 1.0 || rawIndex > static_cast<double>(std::numeric_limits<std::size_t>::max())) {
		return std::nullopt;
	}
	const double rounded = std::round(rawIndex);
	if (std::abs(rawIndex - rounded) > tolerance) {
		return std::nullopt;
	}
	const auto index = static_cast<std::size_t>(rounded);
	return std::abs(term(index) - value) <= tolerance * std::max(1.0, std::abs(value))
		? std::optional<std::size_t>{index}
		: std::nullopt;
}

std::vector<double> ArithmeticSequence::generate(std::size_t termCount) const {
	requireTermCount(termCount);
	std::vector<double> terms;
	terms.reserve(termCount);
	for (std::size_t index = 1; index <= termCount; ++index) {
		terms.push_back(term(index));
	}
	return terms;
}

} // namespace aksiomat::sequences
