#pragma once

#include <string>
#include <vector>

namespace aksiomat::probability_statistics {

struct BinomialResult {
	double probability;
	double mean;
	double variance;
	std::vector<std::string> steps;
};

struct PoissonResult {
	double probability;
	double mean;
	double variance;
	std::vector<std::string> steps;
};

struct NormalResult {
	double density;
	double cumulative;
	double zScore;
	std::vector<std::string> steps;
};

struct UniformResult {
	double density;
	double cumulative;
	double mean;
	double variance;
	std::vector<std::string> steps;
};

// Diskretne i kontinuirane distribucije s pripadnim ocekivanjem i varijancom.
class Distributions {
public:
	static BinomialResult binomial(int trials, double probability, int successes);
	static PoissonResult poisson(double lambda, int occurrences);
	static NormalResult normal(double mean, double standardDeviation, double value);
	static UniformResult uniform(double lowerBound, double upperBound, double value);
};

} // namespace aksiomat::probability_statistics
