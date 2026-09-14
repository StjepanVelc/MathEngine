#include "aksiomat/discrete_math/AdvancedCombinatorics.hpp"

#include <sstream>
#include <stdexcept>

namespace aksiomat::discrete_math {

namespace {

std::string formatNumber(long long value) {
	return std::to_string(value);
}

} // namespace

InclusionExclusionResult AdvancedCombinatorics::unionSizeTwoSets(long long sizeA, long long sizeB, long long sizeIntersection) {
	if (sizeA < 0 || sizeB < 0 || sizeIntersection < 0) {
		throw std::invalid_argument("Velicine skupova ne smiju biti negativne.");
	}
	const long long result = sizeA + sizeB - sizeIntersection;
	std::vector<std::string> steps{
		"Formula ukljucivanja-iskljucivanja za dva skupa: |A ∪ B| = |A| + |B| - |A ∩ B|.",
		"|A ∪ B| = " + formatNumber(sizeA) + " + " + formatNumber(sizeB) + " - " + formatNumber(sizeIntersection) + " = " + formatNumber(result) + "."
	};
	return InclusionExclusionResult{result, std::move(steps)};
}

InclusionExclusionResult AdvancedCombinatorics::unionSizeThreeSets(
	long long sizeA, long long sizeB, long long sizeC,
	long long sizeAB, long long sizeAC, long long sizeBC, long long sizeABC) {
	const long long result = sizeA + sizeB + sizeC - sizeAB - sizeAC - sizeBC + sizeABC;
	std::vector<std::string> steps{
		"Formula ukljucivanja-iskljucivanja za tri skupa: |A ∪ B ∪ C| = |A|+|B|+|C| - |A∩B| - |A∩C| - |B∩C| + |A∩B∩C|.",
		"|A ∪ B ∪ C| = " + formatNumber(result) + "."
	};
	return InclusionExclusionResult{result, std::move(steps)};
}

PigeonholeResult AdvancedCombinatorics::pigeonhole(long long items, long long holes) {
	if (holes <= 0) {
		throw std::invalid_argument("Broj pretinaca mora biti pozitivan.");
	}
	if (items < 0) {
		throw std::invalid_argument("Broj predmeta ne smije biti negativan.");
	}
	const long long minimumPerHole = (items + holes - 1) / holes;
	std::vector<std::string> steps{
		"Dirichletovo nacelo: bar jedan pretinac sadrzi barem ⌈n/k⌉ predmeta.",
		"⌈" + formatNumber(items) + "/" + formatNumber(holes) + "⌉ = " + formatNumber(minimumPerHole) + "."
	};
	return PigeonholeResult{minimumPerHole, std::move(steps)};
}

DerangementResult AdvancedCombinatorics::derangements(long long n) {
	if (n < 0) {
		throw std::invalid_argument("Broj elemenata ne smije biti negativan.");
	}
	// D(n) = n! * sum_{i=0}^{n} (-1)^i / i!, izracunato rekurzijom radi tocnosti s cijelim brojevima.
	std::vector<long long> d(static_cast<std::size_t>(n) + 1, 0);
	d[0] = 1;
	if (n >= 1) d[1] = 0;
	for (long long i = 2; i <= n; ++i) {
		d[static_cast<std::size_t>(i)] = (i - 1) * (d[static_cast<std::size_t>(i - 1)] + d[static_cast<std::size_t>(i - 2)]);
	}
	const long long result = d[static_cast<std::size_t>(n)];

	std::vector<std::string> steps{
		"Broj deranzmana racuna se rekurzijom D(n) = (n-1) * (D(n-1) + D(n-2)), s D(0)=1, D(1)=0.",
		"D(" + formatNumber(n) + ") = " + formatNumber(result) + "."
	};
	return DerangementResult{result, std::move(steps)};
}

} // namespace aksiomat::discrete_math
