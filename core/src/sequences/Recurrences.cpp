#include "aksiomat/sequences/Recurrences.hpp"

#include "aksiomat/sequences/ArithmeticSequence.hpp"
#include "aksiomat/sequences/GeometricSequence.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::sequences {
namespace {

constexpr std::size_t maximumGeneratedTerms = 10000;

void requireCount(std::size_t termCount) {
	if (termCount == 0 || termCount > maximumGeneratedTerms) {
		throw std::invalid_argument("Term count must be between 1 and 10000");
	}
}

void requireFinite(double value, const char* message) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(message);
	}
}

bool equivalent(const std::vector<double>& left, const std::vector<double>& right) {
	if (left.size() != right.size()) {
		return false;
	}
	for (std::size_t i = 0; i < left.size(); ++i) {
		if (std::abs(left[i] - right[i]) > 1e-10 * std::max(1.0, std::abs(left[i]))) {
			return false;
		}
	}
	return true;
}

} // namespace

std::vector<double> Recurrences::arithmetic(double firstTerm, double difference, std::size_t termCount) {
	requireCount(termCount);
	requireFinite(firstTerm, "First term must be finite");
	requireFinite(difference, "Difference must be finite");
	std::vector<double> terms{firstTerm};
	terms.reserve(termCount);
	while (terms.size() < termCount) {
		const double next = terms.back() + difference;
		requireFinite(next, "Arithmetic recurrence overflowed");
		terms.push_back(next);
	}
	return terms;
}

std::vector<double> Recurrences::geometric(double firstTerm, double ratio, std::size_t termCount) {
	requireCount(termCount);
	requireFinite(firstTerm, "First term must be finite");
	requireFinite(ratio, "Ratio must be finite");
	std::vector<double> terms{firstTerm};
	terms.reserve(termCount);
	while (terms.size() < termCount) {
		const double next = terms.back() * ratio;
		requireFinite(next, "Geometric recurrence overflowed");
		terms.push_back(next);
	}
	return terms;
}

std::vector<double> Recurrences::fibonacci(double firstTerm, double secondTerm, std::size_t termCount) {
	requireCount(termCount);
	requireFinite(firstTerm, "First Fibonacci term must be finite");
	requireFinite(secondTerm, "Second Fibonacci term must be finite");
	std::vector<double> terms{firstTerm};
	terms.reserve(termCount);
	if (termCount > 1) {
		terms.push_back(secondTerm);
	}
	while (terms.size() < termCount) {
		const double next = terms[terms.size() - 1] + terms[terms.size() - 2];
		requireFinite(next, "Fibonacci recurrence overflowed");
		terms.push_back(next);
	}
	return terms;
}

RecurrenceComparison Recurrences::compareArithmetic(double firstTerm, double difference, std::size_t termCount) {
	const auto explicitTerms = ArithmeticSequence(firstTerm, difference).generate(termCount);
	const auto recursiveTerms = arithmetic(firstTerm, difference, termCount);
	return {explicitTerms, recursiveTerms, equivalent(explicitTerms, recursiveTerms)};
}

RecurrenceComparison Recurrences::compareGeometric(double firstTerm, double ratio, std::size_t termCount) {
	const auto explicitTerms = GeometricSequence(firstTerm, ratio).generate(termCount);
	const auto recursiveTerms = geometric(firstTerm, ratio, termCount);
	return {explicitTerms, recursiveTerms, equivalent(explicitTerms, recursiveTerms)};
}

} // namespace aksiomat::sequences
