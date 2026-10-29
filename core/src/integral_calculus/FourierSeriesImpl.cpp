#include "aksiomat/integral_calculus/FourierSeries.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::integral_calculus {
namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr int kIntervals = 4000;

std::string fmtNum(double v) {
	char buf[40];
	std::snprintf(buf, sizeof buf, "%.6g", v);
	return buf;
}

} // namespace

FourierSeriesResult FourierSeries::compute(const std::string& expression, double halfPeriod, int terms) {
	if (!std::isfinite(halfPeriod) || halfPeriod <= 0.0) throw std::invalid_argument("Poluperiod L mora biti pozitivan konacan broj.");
	if (terms < 1 || terms > 50) throw std::invalid_argument("Broj clanova mora biti izmedu 1 i 50.");
	const auto f = differential_calculus::parseFunction(expression);

	const int n = kIntervals;
	const double h = 2.0 * halfPeriod / n;
	std::vector<double> fx(n + 1);
	for (int i = 0; i <= n; ++i) {
		fx[i] = differential_calculus::evaluate(f, -halfPeriod + i * h);
		if (!std::isfinite(fx[i])) throw std::invalid_argument("Funkcija nije definirana na cijelom intervalu [-L, L].");
	}

	auto simpson = [&](auto weightFn) {
		double s = 0.0;
		for (int i = 0; i <= n; ++i) {
			const double w = (i == 0 || i == n) ? 1.0 : (i % 2 == 1 ? 4.0 : 2.0);
			s += w * fx[i] * weightFn(-halfPeriod + i * h);
		}
		return s * h / 3.0 / halfPeriod;
	};

	FourierSeriesResult r;
	r.expression = expression;
	r.halfPeriod = halfPeriod;
	r.terms = terms;
	r.a0 = simpson([](double) { return 1.0; });
	for (int k = 1; k <= terms; ++k) {
		const double w = k * kPi / halfPeriod;
		r.a.push_back(simpson([&](double x) { return std::cos(w * x); }));
		r.b.push_back(simpson([&](double x) { return std::sin(w * x); }));
	}

	double err = 0.0;
	for (int i = 0; i <= n; ++i) {
		const double d = fx[i] - partialSum(r, -halfPeriod + i * h);
		const double w = (i == 0 || i == n) ? 1.0 : (i % 2 == 1 ? 4.0 : 2.0);
		err += w * d * d;
	}
	r.meanSquareError = err * h / 3.0 / (2.0 * halfPeriod);

	r.steps = {
		"Funkciju f(x) razvijamo na [-L, L], L = " + fmtNum(halfPeriod) + ", kao S(x) = a0/2 + sum (a_n cos(n pi x / L) + b_n sin(n pi x / L)).",
		"Koeficijenti: a_n = (1/L) int f(x) cos(n pi x / L) dx, b_n = (1/L) int f(x) sin(n pi x / L) dx (Simpson, " + std::to_string(n) + " podintervala).",
		"a0 = " + fmtNum(r.a0) + "; a1 = " + fmtNum(r.a[0]) + "; b1 = " + fmtNum(r.b[0]) + ".",
		"Prosjecna kvadratna pogreska parcijalne sume s " + std::to_string(terms) + " clanova: " + fmtNum(r.meanSquareError) + "."
	};
	return r;
}

double FourierSeries::partialSum(const FourierSeriesResult& s, double x) {
	double v = s.a0 / 2.0;
	for (size_t k = 0; k < s.a.size(); ++k) {
		const double w = static_cast<double>(k + 1) * kPi / s.halfPeriod;
		v += s.a[k] * std::cos(w * x) + s.b[k] * std::sin(w * x);
	}
	return v;
}

} // namespace aksiomat::integral_calculus
