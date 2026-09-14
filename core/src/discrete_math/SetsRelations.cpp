#include "aksiomat/discrete_math/SetsRelations.hpp"

#include <algorithm>
#include <set>
#include <sstream>
#include <stdexcept>

namespace aksiomat::discrete_math {

namespace {

std::string formatSet(const IntSet& values) {
	std::ostringstream stream;
	stream << "{";
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (i) stream << ", ";
		stream << values[i];
	}
	stream << "}";
	return stream.str();
}

std::set<long long> toSet(const IntSet& values) {
	return std::set<long long>(values.begin(), values.end());
}

IntSet fromSet(const std::set<long long>& values) {
	return IntSet(values.begin(), values.end());
}

} // namespace

SetOperationResult SetsRelations::unionOf(const IntSet& a, const IntSet& b) {
	std::set<long long> result = toSet(a);
	const auto setB = toSet(b);
	result.insert(setB.begin(), setB.end());
	IntSet sorted = fromSet(result);
	std::vector<std::string> steps{
		"Unija A i B sadrzi sve elemente koji pripadaju barem jednom od skupova.",
		"A ∪ B = " + formatSet(sorted) + "."
	};
	return SetOperationResult{sorted, std::move(steps)};
}

SetOperationResult SetsRelations::intersectionOf(const IntSet& a, const IntSet& b) {
	const auto setA = toSet(a);
	const auto setB = toSet(b);
	std::set<long long> result;
	std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(result, result.begin()));
	IntSet sorted = fromSet(result);
	std::vector<std::string> steps{
		"Presjek A i B sadrzi elemente koji pripadaju objema skupovima istovremeno.",
		"A ∩ B = " + formatSet(sorted) + "."
	};
	return SetOperationResult{sorted, std::move(steps)};
}

SetOperationResult SetsRelations::differenceOf(const IntSet& a, const IntSet& b) {
	const auto setA = toSet(a);
	const auto setB = toSet(b);
	std::set<long long> result;
	std::set_difference(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(result, result.begin()));
	IntSet sorted = fromSet(result);
	std::vector<std::string> steps{
		"Razlika A \\ B sadrzi elemente iz A koji ne pripadaju B.",
		"A \\ B = " + formatSet(sorted) + "."
	};
	return SetOperationResult{sorted, std::move(steps)};
}

SetOperationResult SetsRelations::symmetricDifferenceOf(const IntSet& a, const IntSet& b) {
	const auto setA = toSet(a);
	const auto setB = toSet(b);
	std::set<long long> result;
	std::set_symmetric_difference(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(result, result.begin()));
	IntSet sorted = fromSet(result);
	std::vector<std::string> steps{
		"Simetricna razlika sadrzi elemente koji pripadaju tocno jednom od skupova.",
		"A △ B = (A \\ B) ∪ (B \\ A) = " + formatSet(sorted) + "."
	};
	return SetOperationResult{sorted, std::move(steps)};
}

RelationPropertiesResult SetsRelations::analyzeRelation(const IntSet& domain, const IntPairs& relation) {
	if (domain.empty()) {
		throw std::invalid_argument("Domena relacije ne smije biti prazna.");
	}
	std::set<std::pair<long long, long long>> pairs(relation.begin(), relation.end());

	bool reflexive = true;
	for (long long x : domain) {
		if (!pairs.count({x, x})) { reflexive = false; break; }
	}

	bool symmetric = true;
	for (const auto& [x, y] : pairs) {
		if (!pairs.count({y, x})) { symmetric = false; break; }
	}

	bool antisymmetric = true;
	for (const auto& [x, y] : pairs) {
		if (x != y && pairs.count({y, x})) { antisymmetric = false; break; }
	}

	bool transitive = true;
	for (const auto& [x, y] : pairs) {
		for (const auto& [y2, z] : pairs) {
			if (y == y2 && !pairs.count({x, z})) { transitive = false; break; }
		}
		if (!transitive) break;
	}

	const bool isEquivalence = reflexive && symmetric && transitive;
	const bool isPartialOrder = reflexive && antisymmetric && transitive;

	std::vector<std::string> steps{
		std::string("Refleksivnost: ") + (reflexive ? "da" : "ne") + " (svaki x mora biti u relaciji sa samim sobom).",
		std::string("Simetricnost: ") + (symmetric ? "da" : "ne") + " ((x,y) u relaciji povlaci (y,x) u relaciji).",
		std::string("Antisimetricnost: ") + (antisymmetric ? "da" : "ne") + " ((x,y) i (y,x) u relaciji povlaci x=y).",
		std::string("Tranzitivnost: ") + (transitive ? "da" : "ne") + " ((x,y) i (y,z) u relaciji povlaci (x,z) u relaciji).",
		std::string("Relacija ekvivalencije: ") + (isEquivalence ? "da" : "ne") + ".",
		std::string("Relacija parcijalnog uredaja: ") + (isPartialOrder ? "da" : "ne") + "."
	};

	return RelationPropertiesResult{reflexive, symmetric, antisymmetric, transitive, isEquivalence, isPartialOrder, std::move(steps)};
}

} // namespace aksiomat::discrete_math
