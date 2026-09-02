#pragma once

namespace aksiomat::combinatorics_probability_statistics {

struct ConditionalProbabilityInput {
	double probabilityAAndB;
	double probabilityB;
};

class ProbabilityBasics {
public:
	static double classicalProbability(double favorableOutcomes, double totalOutcomes);
	static double complementProbability(double probability);
	static double unionProbability(double probabilityA, double probabilityB, double probabilityAAndB);
	static double conditionalProbability(ConditionalProbabilityInput input);
	static double independentEventsProbability(double probabilityA, double probabilityB);
};

} // namespace aksiomat::combinatorics_probability_statistics
