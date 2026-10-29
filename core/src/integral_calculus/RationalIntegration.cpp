#include <algorithm>
#include <cmath>
#include <complex>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/IntegrationInternal.hpp"

namespace aksiomat::integral_calculus::detail {
namespace {

using differential_calculus::formatValue;
using differential_calculus::snapToNice;
using Complex = std::complex<double>;

constexpr double kEps = 1e-9;

void trim(Coefficients& c) {
	while (c.size() > 1 && std::fabs(c.back()) < kEps) c.pop_back();
	if (c.empty()) c.push_back(0.0);
}

// Dijeljenje polinoma: p = q * quotient + remainder.
void divide(const Coefficients& p, const Coefficients& q, Coefficients& quotient, Coefficients& remainder) {
	remainder = p;
	trim(remainder);
	const int dq = polynomialDegree(q);
	const int dp = polynomialDegree(remainder);
	if (dp < dq) {
		quotient = {0.0};
		return;
	}
	quotient.assign(static_cast<std::size_t>(dp - dq + 1), 0.0);
	for (int i = dp; i >= dq; --i) {
		const double factor = remainder[i] / q[dq];
		quotient[i - dq] = factor;
		for (int j = 0; j <= dq; ++j) remainder[i - dq + j] -= factor * q[j];
		remainder[i] = 0.0;
	}
	remainder.resize(static_cast<std::size_t>(std::max(dq, 1)));
	trim(remainder);
}

std::vector<Complex> findRoots(const Coefficients& q) {
	const int n = polynomialDegree(q);
	std::vector<Complex> roots(static_cast<std::size_t>(n));
	auto eval = [&](Complex z) {
		Complex r = q[n];
		for (int i = n - 1; i >= 0; --i) r = r * z + q[i];
		return r;
	};
	double radius = 0.0;
	for (int i = 0; i < n; ++i) radius = std::max(radius, std::fabs(q[i] / q[n]));
	radius = 1.0 + radius;
	for (int i = 0; i < n; ++i) roots[i] = std::polar(radius * 0.5, 0.7 + 2.0 * 3.14159265358979 * i / n);
	for (int iter = 0; iter < 2000; ++iter) {
		double change = 0.0;
		for (int i = 0; i < n; ++i) {
			Complex denom = q[n];
			for (int j = 0; j < n; ++j)
				if (j != i) denom *= roots[i] - roots[j];
			if (std::abs(denom) < 1e-300) continue;
			const Complex delta = eval(roots[i]) / denom;
			roots[i] -= delta;
			change = std::max(change, std::abs(delta));
		}
		if (change < 1e-14) break;
	}
	return roots;
}

struct RealRoot {
	double value;
	int multiplicity;
};
struct QuadFactor {
	double p, q; // x^2 + p x + q
};

bool solveLinear(std::vector<std::vector<double>>& m, std::vector<double>& x) {
	const int n = static_cast<int>(m.size());
	for (int col = 0; col < n; ++col) {
		int pivot = col;
		for (int r = col + 1; r < n; ++r)
			if (std::fabs(m[r][col]) > std::fabs(m[pivot][col])) pivot = r;
		if (std::fabs(m[pivot][col]) < 1e-12) return false;
		std::swap(m[pivot], m[col]);
		for (int r = 0; r < n; ++r) {
			if (r == col) continue;
			const double f = m[r][col] / m[col][col];
			for (int c = col; c <= n; ++c) m[r][c] -= f * m[col][c];
		}
	}
	x.assign(static_cast<std::size_t>(n), 0.0);
	for (int i = 0; i < n; ++i) x[i] = m[i][n] / m[i][i];
	return true;
}

Ptr linearFactor(double r, const std::string& v) {
	return fromPolynomial({-r, 1.0}, v);
}

} // namespace

Outcome integrateRational(const Ptr& f, const std::string& v, int depth) {
	(void)depth;
	const Factors fl = flattenFactors(f, v);
	if (fl.denominators.empty()) return std::nullopt;

	Ptr numerator = num(fl.coefficient);
	for (const Ptr& n : fl.numerators) numerator = mul(numerator, n);
	Ptr denominator;
	for (const Ptr& d : fl.denominators) denominator = denominator ? mul(denominator, d) : d;

	const auto pn = toPolynomial(numerator);
	const auto pd = toPolynomial(denominator);
	if (!pn || !pd) return std::nullopt;
	const int dq = polynomialDegree(*pd);
	if (dq < 1 || dq > 8) return std::nullopt;

	Coefficients quotient, remainder;
	divide(*pn, *pd, quotient, remainder);

	std::vector<std::string> steps;
	steps.push_back("Podintegralna funkcija je racionalna: P(x)/Q(x) = " + show(f) + ". Najprije provjerimo stupnjeve.");
	const bool improper = polynomialDegree(*pn) >= dq;
	if (improper) {
		steps.push_back("stupanj(P) >= stupanj(Q), pa dijelimo polinome: P/Q = " + show(fromPolynomial(quotient, v)) +
			" + (" + show(fromPolynomial(remainder, v)) + ")/(" + show(denominator) + ").");
	}

	Ptr result = integratePolynomialExpr(quotient, v);
	const bool hasQuotient = improper && !(quotient.size() == 1 && std::fabs(quotient[0]) < kEps);
	if (!hasQuotient) result = nullptr;

	const bool remainderZero = remainder.size() == 1 && std::fabs(remainder[0]) < kEps;
	if (!remainderZero) {
		Coefficients q = *pd;
		const double lead = q[dq];
		for (double& c : q) c /= lead;
		Coefficients r = remainder;
		for (double& c : r) c /= lead;

		auto roots = findRoots(q);
		std::vector<RealRoot> real;
		std::vector<QuadFactor> quads;
		std::vector<bool> used(roots.size(), false);
		for (std::size_t i = 0; i < roots.size(); ++i) {
			if (used[i]) continue;
			std::vector<std::size_t> cluster{i};
			for (std::size_t j = i + 1; j < roots.size(); ++j)
				if (!used[j] && std::abs(roots[i] - roots[j]) < 2e-3) cluster.push_back(j);
			Complex mean = 0.0;
			for (auto k : cluster) {
				used[k] = true;
				mean += roots[k];
			}
			mean /= static_cast<double>(cluster.size());
			if (std::fabs(mean.imag()) < 2e-3) {
				real.push_back({snapToNice(mean.real()), static_cast<int>(cluster.size())});
			} else if (mean.imag() > 0.0) {
				const int mult = static_cast<int>(cluster.size());
				if (mult > 1) return std::nullopt;
				quads.push_back({-2.0 * mean.real(), std::norm(mean)});
			}
		}
		int total = 0;
		for (const auto& rr : real) total += rr.multiplicity;
		total += 2 * static_cast<int>(quads.size());
		if (total != dq) return std::nullopt;
		for (auto& qd : quads) {
			qd.p = snapToNice(qd.p);
			qd.q = snapToNice(qd.q);
		}

		struct Basis {
			Coefficients poly;
		};
		std::vector<Basis> basis;
		auto cofactor = [&](const Coefficients& factor) {
			Coefficients quo, rem;
			divide(q, factor, quo, rem);
			return quo;
		};
		for (const auto& rr : real) {
			Coefficients factor{1.0};
			for (int k = 0; k < rr.multiplicity; ++k) {
				Coefficients lin{-rr.value, 1.0};
				Coefficients next(factor.size() + 1, 0.0);
				for (std::size_t a = 0; a < factor.size(); ++a)
					for (std::size_t b = 0; b < 2; ++b) next[a + b] += factor[a] * lin[b];
				factor = next;
				basis.push_back({cofactor(factor)});
			}
		}
		for (const auto& qd : quads) {
			const Coefficients quad{qd.q, qd.p, 1.0};
			Coefficients c = cofactor(quad);
			Coefficients xc(c.size() + 1, 0.0);
			for (std::size_t i = 0; i < c.size(); ++i) xc[i + 1] = c[i];
			basis.push_back({xc});
			basis.push_back({c});
		}
		if (static_cast<int>(basis.size()) != dq) return std::nullopt;

		std::vector<std::vector<double>> m(static_cast<std::size_t>(dq), std::vector<double>(static_cast<std::size_t>(dq) + 1, 0.0));
		for (int row = 0; row < dq; ++row) {
			for (int col = 0; col < dq; ++col)
				m[row][col] = row < static_cast<int>(basis[col].poly.size()) ? basis[col].poly[row] : 0.0;
			m[row][dq] = row < static_cast<int>(r.size()) ? r[row] : 0.0;
		}
		std::vector<double> sol;
		if (!solveLinear(m, sol)) return std::nullopt;

		std::string decomposition;
		int idx = 0;
		Ptr integral;
		auto append = [&](const Ptr& term, double sign) {
			integral = integral ? (sign < 0.0 ? sub(integral, term) : add(integral, term))
								: (sign < 0.0 ? neg(term) : term);
		};
		auto appendText = [&](const std::string& t) { decomposition += (decomposition.empty() ? "" : " + ") + t; };

		for (const auto& rr : real) {
			for (int k = 1; k <= rr.multiplicity; ++k) {
				const double A = snapToNice(sol[idx++]);
				if (std::fabs(A) < 1e-9) continue;
				const Ptr base = linearFactor(rr.value, v);
				appendText(formatValue(A) + "/(" + show(base) + (k > 1 ? ")^" + std::to_string(k) : ")"));
				if (k == 1) {
					append(scaleBy(std::fabs(A), lnAbs(base)), A);
				} else {
					append(div(num(std::fabs(A)), mul(num(k - 1.0), pow(base, num(k - 1.0)))), -A);
				}
			}
		}
		for (const auto& qd : quads) {
			const double B = snapToNice(sol[idx++]);
			const double C = snapToNice(sol[idx++]);
			const Ptr quad = fromPolynomial({qd.q, qd.p, 1.0}, v);
			appendText("(" + formatValue(B) + "x + " + formatValue(C) + ")/(" + show(quad) + ")");
			const double D = C - B * qd.p / 2.0;
			const double disc = qd.q - qd.p * qd.p / 4.0;
			if (disc <= 0.0) return std::nullopt;
			if (std::fabs(B) > 1e-9) append(scaleBy(std::fabs(B) / 2.0, call(FnFunc::Ln, quad)), B);
			if (std::fabs(D) > 1e-9) {
				const double s = std::sqrt(disc);
				const Ptr arg = fromPolynomial({qd.p / (2.0 * s), 1.0 / s}, v);
				append(scaleBy(std::fabs(D) / s, call(FnFunc::Atan, arg)), D);
			}
		}
		steps.push_back("Nazivnik rastavimo na faktore (realne nultocke daju linearne faktore, a konjugirano "
			"kompleksne nultocke nerastavljive kvadratne). Rastav na parcijalne razlomke: " + decomposition +
			". Koeficijente nalazimo izjednacavanjem koeficijenata uz iste potencije od x.");
		steps.push_back("Svaki parcijalni razlomak integriramo tablicno: A/(x-a) daje A·ln|x-a|, A/(x-a)^k daje "
			"-A/((k-1)(x-a)^(k-1)), a (Bx+C)/(x^2+px+q) daje (B/2)·ln(x^2+px+q) + arctg dio.");
		if (!integral) integral = num(0.0);
		result = result ? add(result, integral) : integral;
	}
	if (!result) return std::nullopt;
	Attempt a;
	a.antiderivative = result;
	a.steps = steps;
	a.method = "racionalna funkcija";
	return a;
}

} // namespace aksiomat::integral_calculus::detail
