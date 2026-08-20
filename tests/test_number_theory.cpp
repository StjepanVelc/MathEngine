#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/arithmetic/NumberTheory.hpp"

using aksiomat::NumberTheory;

TEST(NumberTheory, DetectsPrimes) {
	EXPECT_FALSE(NumberTheory::isPrime(0));
	EXPECT_FALSE(NumberTheory::isPrime(1));
	EXPECT_TRUE(NumberTheory::isPrime(2));
	EXPECT_TRUE(NumberTheory::isPrime(97));
	EXPECT_FALSE(NumberTheory::isPrime(100));
}

TEST(NumberTheory, DetectsLcmUint64Overflow) {
	const auto maximum = std::numeric_limits<std::uint64_t>::max();
	EXPECT_THROW(NumberTheory::lcm(maximum, 2), std::overflow_error);
}

TEST(NumberTheory, ListsSortedDivisors) {
	EXPECT_EQ(NumberTheory::divisors(12), (std::vector<std::uint64_t>{1, 2, 3, 4, 6, 12}));
	EXPECT_THROW(NumberTheory::divisors(0), std::invalid_argument);
}

TEST(NumberTheory, FactorizesNumber) {
	const auto factors = NumberTheory::factorize(360);
	ASSERT_EQ(factors.size(), 3u);
	EXPECT_EQ(factors[0].prime, 2u);
	EXPECT_EQ(factors[0].exponent, 3u);
	EXPECT_EQ(factors[1].prime, 3u);
	EXPECT_EQ(factors[1].exponent, 2u);
	EXPECT_EQ(factors[2].prime, 5u);
	EXPECT_EQ(factors[2].exponent, 1u);
}

TEST(NumberTheory, CalculatesGcdAndLcm) {
	EXPECT_EQ(NumberTheory::gcd(48, 18), 6u);
	EXPECT_EQ(NumberTheory::lcm(12, 18), 36u);
	EXPECT_EQ(NumberTheory::lcm(0, 18), 0u);
}
