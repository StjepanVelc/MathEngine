#pragma once

#include <cstdint>
#include <vector>

namespace aksiomat {

struct PrimeFactor {
	std::uint64_t prime;
	unsigned exponent;
};

class NumberTheory {
public:
	static bool isPrime(std::uint64_t value);
	static std::vector<std::uint64_t> divisors(std::uint64_t value);
	static std::vector<PrimeFactor> factorize(std::uint64_t value);
	static std::uint64_t gcd(std::uint64_t lhs, std::uint64_t rhs);
	static std::uint64_t lcm(std::uint64_t lhs, std::uint64_t rhs);
};

} // namespace aksiomat
