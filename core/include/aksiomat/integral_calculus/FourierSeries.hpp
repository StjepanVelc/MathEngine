#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct FourierSeriesResult {
	std::string expression;
	double halfPeriod = 3.14159265358979323846; // L: razvoj na [-L, L]
	int terms = 0;
	double a0 = 0.0;                 // slobodni clan a0/2
	std::vector<double> a;           // a1..aN
	std::vector<double> b;           // b1..bN
	double meanSquareError = 0.0;    // prosjecna kvadratna pogreska parcijalne sume na [-L, L]
	std::vector<std::string> steps;
};

class FourierSeries {
public:
	// Trigonometrijski Fourierov red funkcije f na [-L, L]: koeficijenti a_n, b_n (Simpson) i procjena pogreske.
	// Baca std::invalid_argument za neispravan izraz, L <= 0 ili terms izvan [1, 50].
	static FourierSeriesResult compute(const std::string& expression, double halfPeriod, int terms);

	// Vrijednost parcijalne sume S_N(x) = a0/2 + sum (a_n cos(n pi x / L) + b_n sin(n pi x / L)).
	static double partialSum(const FourierSeriesResult& series, double x);
};

} // namespace aksiomat::integral_calculus
