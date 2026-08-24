#pragma once

#include <cstddef>
#include <optional>
#include <vector>

namespace aksiomat::sequences {

struct InfiniteSeriesResult {
	bool converges;
	std::optional<double> sum;
};

class GeometricSequence {
public:
	GeometricSequence(double firstTerm, double ratio);

	double firstTerm() const noexcept;
	double ratio() const noexcept;
	double term(std::size_t index) const;
	double partialSum(std::size_t termCount) const;
	std::optional<std::size_t> indexOf(double value, std::size_t maxIndex = 10000, double tolerance = 1e-9) const;
	InfiniteSeriesResult infiniteSum() const;
	std::vector<double> generate(std::size_t termCount) const;

private:
	double firstTerm_;
	double ratio_;
};

} // namespace aksiomat::sequences
