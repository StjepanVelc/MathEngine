#pragma once

#include <string>

namespace aksiomat::integral_calculus {

// JSON serijalizacija integralnog racuna za web sloj. NaN i +-inf zapisuju se kao null.
// Sve funkcije bacaju std::invalid_argument za neispravan unos (WASM sloj to pretvara u "GRESKA: ...").
class IntegralCalculusJson {
public:
	static std::string riemann(const std::string& expression, double a, double b, int n, const std::string& method);
	static std::string indefinite(const std::string& expression);
	static std::string definite(const std::string& expression, double a, double b);
	static std::string areaBetween(const std::string& f, const std::string& g, double a, double b);
	// kind: "upper" (a, +inf), "lower" (-inf, b), "both" (-inf, +inf), "singular_lower" ili "singular_upper" (a, b),
	// "ptest_infinity" ili "ptest_zero" (p se predaje preko a).
	static std::string improper(const std::string& expression, const std::string& kind, double a, double b);
	static std::string meanValue(const std::string& expression, double a, double b);
	// kind: "separable" (p1 = g(x), p2 = h(y)), "linear" (p1 = p(x), p2 = q(x)),
	// "growth" (a = k, b = y0), "logistic" (a = r, b = y0, capacity).
	static std::string ode(const std::string& kind, const std::string& p1, const std::string& p2, bool hasInitial,
		double x0, double y0, double xEnd, double extra);
	static std::string induction(const std::string& kind, const std::string& left, const std::string& right, int n0, int maxN);
	static std::string sequence(const std::string& expression, double epsilon, int termCount);
	static std::string sample(const std::string& expression, double xMin, double xMax, int count);
	// kind: "volume" (volumen rotacije oko osi x) ili "arc" (duljina luka).
	static std::string application(const std::string& kind, const std::string& expression, double a, double b);
	// dimension 2 ili 3; granice u b[0..5] = x0, x1, y0, y1, z0, z1 (z se ignorira za 2).
	static std::string multiple(const std::string& expression, int dimension, double x0, double x1, double y0, double y1, double z0, double z1, int n);
	static std::string fourier(const std::string& expression, double halfPeriod, int terms, int sampleCount);
};

} // namespace aksiomat::integral_calculus
