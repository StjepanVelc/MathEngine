#pragma once

#include <string>
#include <vector>

namespace aksiomat::discrete_math {

struct InclusionExclusionResult {
	long long unionSize;
	std::vector<std::string> steps;
};

struct PigeonholeResult {
	long long minimumPerHole;
	std::vector<std::string> steps;
};

struct DerangementResult {
	long long derangements;
	std::vector<std::string> steps;
};

class AdvancedCombinatorics {
public:
	// Ukljucivanje-iskljucivanje za dva i tri skupa.
	static InclusionExclusionResult unionSizeTwoSets(long long sizeA, long long sizeB, long long sizeIntersection);
	static InclusionExclusionResult unionSizeThreeSets(
		long long sizeA, long long sizeB, long long sizeC,
		long long sizeAB, long long sizeAC, long long sizeBC, long long sizeABC);
	static PigeonholeResult pigeonhole(long long items, long long holes);
	static DerangementResult derangements(long long n);
};

} // namespace aksiomat::discrete_math
