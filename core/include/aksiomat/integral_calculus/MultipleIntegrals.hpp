#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct MultipleIntegralResult {
	std::string expression;
	int dimension = 2;
	double value = 0.0;
	double volume = 0.0;      // mjera podrucja (povrsina ili volumen kvadra)
	double mean = 0.0;        // srednja vrijednost funkcije na podrucju
	int subdivisions = 0;     // broj podintervala po osi
	std::vector<std::string> steps;
};

class MultipleIntegrals {
public:
	// Dvostruki integral f(x, y) dA nad [x0, x1] x [y0, y1] (Fubini + kompozitna Simpsonova formula).
	// Baca std::invalid_argument za neispravan izraz ili granice.
	static MultipleIntegralResult doubleIntegral(const std::string& expression,
		double x0, double x1, double y0, double y1, int n = 40);

	// Trostruki integral f(x, y, z) dV nad kvadrom [x0, x1] x [y0, y1] x [z0, z1].
	static MultipleIntegralResult tripleIntegral(const std::string& expression,
		double x0, double x1, double y0, double y1, double z0, double z1, int n = 16);
};

} // namespace aksiomat::integral_calculus
