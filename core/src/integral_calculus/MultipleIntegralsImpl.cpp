#include "aksiomat/integral_calculus/MultipleIntegrals.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::evaluate;
using differential_calculus::parseFunction;

std::string fmtNum(double v) {
	char buf[40];
	std::snprintf(buf, sizeof buf, "%.6g", v);
	return buf;
}

// Parser poznaje samo varijablu x, pa y i z zamjenjujemo brojem u tekstu
// (nijedno ime funkcije ne sadrzi slovo y ni z).
std::string bind(const std::string& text, char name, double value) {
	char buf[48];
	std::snprintf(buf, sizeof buf, "(%.17g)", value);
	std::string out;
	for (char c : text) {
		if (c == name) out += buf;
		else out += c;
	}
	return out;
}

void check(double a, double b, const char* axis) {
	if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument(std::string("Granice za os ") + axis + " moraju biti konacne.");
	if (!(a < b)) throw std::invalid_argument(std::string("Za os ") + axis + " mora vrijediti donja < gornja granica.");
}

int evenN(int n, int maxN) {
	if (n < 2) n = 2;
	if (n > maxN) n = maxN;
	return n % 2 == 0 ? n : n - 1;
}

double weight(int i, int n) {
	if (i == 0 || i == n) return 1.0;
	return i % 2 == 1 ? 4.0 : 2.0;
}

// Simpson po x za zadani (vec vezani) izraz.
double simpsonX(const std::string& bound, double x0, double hx, int n) {
	const auto f = parseFunction(bound, "x");
	double s = 0.0;
	for (int i = 0; i <= n; ++i) {
		const double v = evaluate(f, x0 + i * hx);
		if (!std::isfinite(v)) throw std::invalid_argument("Funkcija nije definirana na cijelom podrucju integracije.");
		s += weight(i, n) * v;
	}
	return s * hx / 3.0;
}

void finish(MultipleIntegralResult& r, double value, double measure) {
	r.value = value;
	r.volume = measure;
	r.mean = value / measure;
}

} // namespace

MultipleIntegralResult MultipleIntegrals::doubleIntegral(const std::string& expression,
	double x0, double x1, double y0, double y1, int n) {
	check(x0, x1, "x");
	check(y0, y1, "y");
	n = evenN(n, 200);
	const double hx = (x1 - x0) / n;
	const double hy = (y1 - y0) / n;

	double outer = 0.0;
	for (int j = 0; j <= n; ++j)
		outer += weight(j, n) * simpsonX(bind(expression, 'y', y0 + j * hy), x0, hx, n);
	outer *= hy / 3.0;

	MultipleIntegralResult r;
	r.expression = expression;
	r.dimension = 2;
	r.subdivisions = n;
	finish(r, outer, (x1 - x0) * (y1 - y0));
	r.steps = {
		"Podrucje R = [" + fmtNum(x0) + ", " + fmtNum(x1) + "] x [" + fmtNum(y0) + ", " + fmtNum(y1) + "]; po Fubinijevu teoremu iint f dA = int_y int_x f(x,y) dx dy.",
		"Unutarnji i vanjski integral racunaju se kompozitnom Simpsonovom formulom s n = " + std::to_string(n) + " podintervala po osi.",
		"Povrsina podrucja |R| = " + fmtNum(r.volume) + ".",
		"Vrijednost integrala = " + fmtNum(r.value) + "; srednja vrijednost f na R = " + fmtNum(r.mean) + "."
	};
	return r;
}

MultipleIntegralResult MultipleIntegrals::tripleIntegral(const std::string& expression,
	double x0, double x1, double y0, double y1, double z0, double z1, int n) {
	check(x0, x1, "x");
	check(y0, y1, "y");
	check(z0, z1, "z");
	n = evenN(n, 40);
	const double hx = (x1 - x0) / n;
	const double hy = (y1 - y0) / n;
	const double hz = (z1 - z0) / n;

	double outer = 0.0;
	for (int k = 0; k <= n; ++k) {
		const std::string bz = bind(expression, 'z', z0 + k * hz);
		double mid = 0.0;
		for (int j = 0; j <= n; ++j)
			mid += weight(j, n) * simpsonX(bind(bz, 'y', y0 + j * hy), x0, hx, n);
		outer += weight(k, n) * mid * hy / 3.0;
	}
	outer *= hz / 3.0;

	MultipleIntegralResult r;
	r.expression = expression;
	r.dimension = 3;
	r.subdivisions = n;
	finish(r, outer, (x1 - x0) * (y1 - y0) * (z1 - z0));
	r.steps = {
		"Kvadar B = [" + fmtNum(x0) + ", " + fmtNum(x1) + "] x [" + fmtNum(y0) + ", " + fmtNum(y1) + "] x [" + fmtNum(z0) + ", " + fmtNum(z1) + "]; iiint f dV = int_z int_y int_x f dx dy dz (Fubini).",
		"Tri ugnijezdena integrala racunaju se Simpsonovom formulom s n = " + std::to_string(n) + " podintervala po osi.",
		"Volumen kvadra |B| = " + fmtNum(r.volume) + ".",
		"Vrijednost integrala = " + fmtNum(r.value) + "; srednja vrijednost f na B = " + fmtNum(r.mean) + "."
	};
	return r;
}

} // namespace aksiomat::integral_calculus
