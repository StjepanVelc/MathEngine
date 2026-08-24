#pragma once

#include <cstddef>
#include <vector>

namespace aksiomat::sequences {

enum class Monotonicity {
	StrictlyIncreasing,
	NonDecreasing,
	Constant,
	NonIncreasing,
	StrictlyDecreasing,
	NotMonotonic
};

struct SequencePoint {
	std::size_t index;
	double value;
};

struct SequenceSummary {
	std::vector<SequencePoint> points;
	std::vector<double> differences;
	Monotonicity monotonicity;
	double minimum;
	double maximum;
};

class SequenceAnalysis {
public:
	static SequenceSummary analyze(const std::vector<double>& terms);
};

} // namespace aksiomat::sequences
