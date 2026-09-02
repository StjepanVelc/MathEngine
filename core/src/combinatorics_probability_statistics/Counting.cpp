#include "aksiomat/combinatorics_probability_statistics/Counting.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace aksiomat::combinatorics_probability_statistics {
namespace {

void requireNonNegative(long long value, const char* name) {
	if (value < 0) throw std::invalid_argument(std::string(name) + " ne smije biti negativan");
}

void requireKNotGreaterThanN(long long n, long long k) {
	if (k > n) throw std::invalid_argument("k ne smije biti veci od n");
}

} // namespace

double Counting::factorial(long long n) {
	requireNonNegative(n, "n");
	double result = 1.0;
	for (long long i = 2; i <= n; ++i) result *= static_cast<double>(i);
	return result;
}

double Counting::permutations(long long n, long long k) {
	requireNonNegative(n, "n");
	requireNonNegative(k, "k");
	requireKNotGreaterThanN(n, k);
	double result = 1.0;
	for (long long i = 0; i < k; ++i) result *= static_cast<double>(n - i);
	return result;
}

double Counting::permutationsWithRepetition(long long n, long long k) {
	requireNonNegative(n, "n");
	requireNonNegative(k, "k");
	if (n == 0 && k > 0) throw std::invalid_argument("n mora biti pozitivan kada je k pozitivan");
	return std::pow(static_cast<double>(n), static_cast<double>(k));
}

double Counting::combinations(long long n, long long k) {
	requireNonNegative(n, "n");
	requireNonNegative(k, "k");
	requireKNotGreaterThanN(n, k);
	const long long effectiveK = (k > n - k) ? (n - k) : k;
	double result = 1.0;
	for (long long i = 0; i < effectiveK; ++i) {
		result *= static_cast<double>(n - i);
		result /= static_cast<double>(i + 1);
	}
	return result;
}

double Counting::combinationsWithRepetition(long long n, long long k) {
	requireNonNegative(n, "n");
	requireNonNegative(k, "k");
	if (n == 0 && k > 0) throw std::invalid_argument("n mora biti pozitivan kada je k pozitivan");
	if (n == 0 && k == 0) return 1.0;
	return combinations(n + k - 1, k);
}

} // namespace aksiomat::combinatorics_probability_statistics
