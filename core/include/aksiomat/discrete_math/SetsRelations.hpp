#pragma once

#include <string>
#include <vector>

namespace aksiomat::discrete_math {

using IntSet = std::vector<long long>;
using IntPairs = std::vector<std::pair<long long, long long>>;

struct SetOperationResult {
	IntSet result;
	std::vector<std::string> steps;
};

struct RelationPropertiesResult {
	bool reflexive;
	bool symmetric;
	bool antisymmetric;
	bool transitive;
	bool isEquivalence;
	bool isPartialOrder;
	std::vector<std::string> steps;
};

class SetsRelations {
public:
	static SetOperationResult unionOf(const IntSet& a, const IntSet& b);
	static SetOperationResult intersectionOf(const IntSet& a, const IntSet& b);
	static SetOperationResult differenceOf(const IntSet& a, const IntSet& b);
	static SetOperationResult symmetricDifferenceOf(const IntSet& a, const IntSet& b);
	static RelationPropertiesResult analyzeRelation(const IntSet& domain, const IntPairs& relation);
};

} // namespace aksiomat::discrete_math
