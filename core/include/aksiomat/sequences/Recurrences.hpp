#pragma once

#include <cstddef>
#include <vector>

namespace aksiomat::sequences {

struct RecurrenceComparison {
	std::vector<double> explicitTerms;
	std::vector<double> recursiveTerms;
	bool equivalent;
};

class Recurrences {
public:
	static std::vector<double> arithmetic(double firstTerm, double difference, std::size_t termCount);
	static std::vector<double> geometric(double firstTerm, double ratio, std::size_t termCount);
	static std::vector<double> fibonacci(double firstTerm, double secondTerm, std::size_t termCount);
	static RecurrenceComparison compareArithmetic(double firstTerm, double difference, std::size_t termCount);
	static RecurrenceComparison compareGeometric(double firstTerm, double ratio, std::size_t termCount);
};

} // namespace aksiomat::sequences
