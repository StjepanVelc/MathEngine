#include "aksiomat/arithmetic/NumberTheory.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace aksiomat {

bool NumberTheory::isPrime(std::uint64_t value) {
	if (value < 2) return false;
	if (value % 2 == 0) return value == 2;
	for (std::uint64_t divisor = 3; divisor <= value / divisor; divisor += 2) {
		if (value % divisor == 0) return false;
	}
	return true;
}

std::vector<std::uint64_t> NumberTheory::divisors(std::uint64_t value) {
	if (value == 0) throw std::invalid_argument("Nula ima beskonacno mnogo djelitelja");
	std::vector<std::uint64_t> lower;
	std::vector<std::uint64_t> upper;
	for (std::uint64_t divisor = 1; divisor <= value / divisor; ++divisor) {
		if (value % divisor != 0) continue;
		lower.push_back(divisor);
		if (divisor != value / divisor) upper.push_back(value / divisor);
	}
	lower.insert(lower.end(), upper.rbegin(), upper.rend());
	return lower;
}

std::vector<PrimeFactor> NumberTheory::factorize(std::uint64_t value) {
	if (value < 2) throw std::invalid_argument("Faktorizacija zahtijeva broj veci ili jednak 2");
	std::vector<PrimeFactor> factors;
	for (std::uint64_t prime = 2; prime <= value / prime; prime += prime == 2 ? 1 : 2) {
		if (value % prime != 0) continue;
		unsigned exponent = 0;
		do {
			value /= prime;
			++exponent;
		} while (value % prime == 0);
		factors.push_back({prime, exponent});
	}
	if (value > 1) factors.push_back({value, 1});
	return factors;
}

std::uint64_t NumberTheory::gcd(std::uint64_t lhs, std::uint64_t rhs) {
	return std::gcd(lhs, rhs);
}

std::uint64_t NumberTheory::lcm(std::uint64_t lhs, std::uint64_t rhs) {
	if (lhs == 0 || rhs == 0) return 0;
	const auto divisor = gcd(lhs, rhs);
	const auto reduced = lhs / divisor;
	if (reduced > std::numeric_limits<std::uint64_t>::max() / rhs) {
		throw std::overflow_error("NZV prelazi 64-bitni raspon");
	}
	return reduced * rhs;
}

} // namespace aksiomat
