#pragma once

#include <string>
#include <vector>

namespace aksiomat::linear_algebra {

struct LinearIndependenceResult {
	bool linearlyIndependent;
	int rank;
	std::vector<std::string> steps;
};

struct RankResult {
	int rank;
	std::vector<std::string> steps;
};

struct BasisResult {
	std::vector<std::vector<double>> basisVectors;
	int dimension;
	std::vector<std::string> steps;
};

class VectorSpaces {
public:
	// Prima skup vektora iste duljine i Gaussovom eliminacijom odredjuje rang/nezavisnost/bazu.
	static LinearIndependenceResult checkLinearIndependence(const std::vector<std::vector<double>>& vectors);
	static RankResult computeRank(const std::vector<std::vector<double>>& vectors);
	static BasisResult extractBasis(const std::vector<std::vector<double>>& vectors);
};

} // namespace aksiomat::linear_algebra
