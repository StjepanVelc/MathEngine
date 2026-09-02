#include "aksiomat/combinatorics_probability_statistics/ProbabilityBasics.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::combinatorics_probability_statistics {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Vrijednost mora biti konacna");
}

void requireProbabilityRange(double probability) {
	requireFinite(probability);
	if (probability < 0.0 || probability > 1.0) throw std::invalid_argument("Vjerojatnost mora biti u rasponu [0,1]");
}

} // namespace

double ProbabilityBasics::classicalProbability(double favorableOutcomes, double totalOutcomes) {
	requireFinite(favorableOutcomes);
	requireFinite(totalOutcomes);
	if (totalOutcomes <= 0.0) throw std::invalid_argument("Ukupan broj ishoda mora biti pozitivan");
	if (favorableOutcomes < 0.0 || favorableOutcomes > totalOutcomes) throw std::invalid_argument("Povoljni ishodi moraju biti u rasponu [0, ukupno]");
	return favorableOutcomes / totalOutcomes;
}

double ProbabilityBasics::complementProbability(double probability) {
	requireProbabilityRange(probability);
	return 1.0 - probability;
}

double ProbabilityBasics::unionProbability(double probabilityA, double probabilityB, double probabilityAAndB) {
	requireProbabilityRange(probabilityA);
	requireProbabilityRange(probabilityB);
	requireProbabilityRange(probabilityAAndB);
	if (probabilityAAndB > probabilityA || probabilityAAndB > probabilityB) throw std::invalid_argument("P(A i B) ne smije biti veci od P(A) ili P(B)");
	return probabilityA + probabilityB - probabilityAAndB;
}

double ProbabilityBasics::conditionalProbability(ConditionalProbabilityInput input) {
	requireProbabilityRange(input.probabilityAAndB);
	requireProbabilityRange(input.probabilityB);
	if (input.probabilityB <= 0.0) throw std::invalid_argument("P(B) mora biti pozitivan");
	if (input.probabilityAAndB > input.probabilityB) throw std::invalid_argument("P(A i B) ne smije biti veci od P(B)");
	return input.probabilityAAndB / input.probabilityB;
}

double ProbabilityBasics::independentEventsProbability(double probabilityA, double probabilityB) {
	requireProbabilityRange(probabilityA);
	requireProbabilityRange(probabilityB);
	return probabilityA * probabilityB;
}

} // namespace aksiomat::combinatorics_probability_statistics
