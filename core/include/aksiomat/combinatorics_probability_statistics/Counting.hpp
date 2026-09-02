#pragma once

namespace aksiomat::combinatorics_probability_statistics {

class Counting {
public:
	static double factorial(long long n);
	static double permutations(long long n, long long k);
	static double permutationsWithRepetition(long long n, long long k);
	static double combinations(long long n, long long k);
	static double combinationsWithRepetition(long long n, long long k);
};

} // namespace aksiomat::combinatorics_probability_statistics
