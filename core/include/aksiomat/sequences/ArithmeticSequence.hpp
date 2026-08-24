#pragma once

#include <cstddef>
#include <optional>
#include <vector>

namespace aksiomat::sequences {

class ArithmeticSequence {
public:
	ArithmeticSequence(double firstTerm, double difference);

	double firstTerm() const noexcept;
	double difference() const noexcept;
	double term(std::size_t index) const;
	double partialSum(std::size_t termCount) const;
	std::optional<std::size_t> indexOf(double value, double tolerance = 1e-9) const;
	std::vector<double> generate(std::size_t termCount) const;

private:
	double firstTerm_;
	double difference_;
};

} // namespace aksiomat::sequences
