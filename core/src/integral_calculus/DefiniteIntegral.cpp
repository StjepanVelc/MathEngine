#include "aksiomat/integral_calculus/DefiniteIntegral.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/IndefiniteIntegral.hpp"
#include "aksiomat/integral_calculus/RiemannSums.hpp"

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::evaluate;
using differential_calculus::formatValue;
using differential_calculus::parseFunction;

double simpsonValue(const std::string& expression, double a, double b, const std::string& variable) {
	return RiemannSums::compute(expression, a, b, 2000, RiemannMethod::Simpson, variable).approximation;
}

std::string wrap(const std::string& s) { return "(" + s + ")"; }

// Integral od `finite` do `finite + signedT` za velike |t|: antiderivacija ako postoji, inace Simpson po dekadama.
double wideIntegral(const std::string& expression, double finite, double signedT) {
	const auto anti = IndefiniteIntegration::integrate(expression);
	if (anti.found) {
		const auto F = parseFunction(anti.antiderivative);
		const double value = evaluate(F, finite + signedT) - evaluate(F, finite);
		if (std::isfinite(value)) return value;
	}
	const double sign = signedT < 0.0 ? -1.0 : 1.0;
	const double total = std::fabs(signedT);
	double sum = 0.0;
	double from = 0.0;
	double to = 1.0;
	while (from < total) {
		const double end = std::min(to, total);
		const double x0 = finite + sign * from;
		const double x1 = finite + sign * end;
		sum += sign * simpsonValue(expression, std::min(x0, x1), std::max(x0, x1), "x");
		from = end;
		to *= 10.0;
	}
	return sum;
}

} // namespace

DefiniteIntegralResult DefiniteIntegration::integrate(const std::string& expression, double a, double b,
	const std::string& variable) {
	if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument("Granice moraju biti konacni brojevi.");
	const auto f = parseFunction(expression, variable);
	DefiniteIntegralResult r;
	r.expression = differential_calculus::toString(differential_calculus::simplify(f));
	r.lower = a;
	r.upper = b;
	if (a == b) {
		r.method = "jednake granice";
		r.steps.push_back("Granice su jednake pa je integral 0.");
		return r;
	}

	const bool swapped = a > b;
	const double lo = swapped ? b : a;
	const double hi = swapped ? a : b;
	if (swapped) r.steps.push_back("Gornja granica je manja od donje: ∫_a^b f = -∫_b^a f, zamijenimo granice i promijenimo predznak.");

	bool numericOk = true;
	try {
		r.numericCheck = simpsonValue(expression, lo, hi, variable);
	} catch (const std::exception&) {
		numericOk = false;
	}

	const auto anti = IndefiniteIntegration::integrate(expression, variable);
	if (anti.found) {
		const auto F = parseFunction(anti.antiderivative, variable);
		const double Fb = evaluate(F, hi);
		const double Fa = evaluate(F, lo);
		if (std::isfinite(Fa) && std::isfinite(Fb)) {
			const double value = Fb - Fa;
			const bool agrees = numericOk && std::fabs(value - r.numericCheck) <= 1e-4 * (1.0 + std::fabs(value));
			if (agrees) {
				r.symbolic = true;
				r.antiderivative = anti.antiderivative;
				r.value = swapped ? -value : value;
				r.agrees = numericOk;
				r.method = "Newton-Leibniz";
				r.steps.push_back("Najprije nademo antiderivaciju F(x) = " + anti.antiderivative + " (" + anti.method + ").");
				r.steps.push_back("Newton-Leibnizova formula: ∫_a^b f(x) dx = F(b) - F(a).");
				r.steps.push_back("F(" + formatValue(hi) + ") = " + formatValue(Fb) + ", F(" + formatValue(lo) + ") = " +
					formatValue(Fa) + ", pa je F(b) - F(a) = " + formatValue(value) + ".");
				if (numericOk)
					r.steps.push_back("Neovisna numericka provjera (Simpson, n = 2000) daje " + formatValue(r.numericCheck) +
						", sto se slaze s rezultatom.");
				if (swapped) r.steps.push_back("Zbog zamjene granica konacni rezultat je " + formatValue(r.value) + ".");
				return r;
			}
			r.steps.push_back("Antiderivacija F(x) = " + anti.antiderivative + " ne daje vrijednost koja se slaze s numerikom "
				"(vjerojatno funkcija ima prekid unutar intervala), pa koristimo numericku integraciju.");
		} else {
			r.steps.push_back("F nije definirana u rubu intervala, pa koristimo numericku integraciju.");
		}
	} else {
		r.steps.push_back("Antiderivacija nije pronadena elementarnim metodama pa integral procjenjujemo numericki.");
	}

	if (!numericOk) throw std::invalid_argument("Integral nije moguce izracunati: funkcija nije definirana na cijelom intervalu.");
	r.method = "numericka integracija (Simpson)";
	r.value = swapped ? -r.numericCheck : r.numericCheck;
	r.agrees = false;
	r.steps.push_back("Simpsonova metoda s n = 2000 daje " + formatValue(r.value) + ".");
	return r;
}

AreaBetweenCurvesResult DefiniteIntegration::areaBetween(const std::string& f, const std::string& g, double a, double b) {
	if (!std::isfinite(a) || !std::isfinite(b) || !(a < b)) throw std::invalid_argument("Potrebno je a < b.");
	const auto fe = parseFunction(f);
	const auto ge = parseFunction(g);
	AreaBetweenCurvesResult r;
	r.upperCurve = f;
	r.lowerCurve = g;
	r.lower = a;
	r.upper = b;

	auto diff = [&](double x) { return evaluate(fe, x) - evaluate(ge, x); };
	std::vector<double> cuts{a};
	const int n = 4000;
	double prevX = a, prev = diff(a);
	for (int i = 1; i <= n; ++i) {
		const double x = a + (b - a) * i / n;
		const double cur = diff(x);
		if (std::isfinite(prev) && std::isfinite(cur) && prev * cur < 0.0) {
			double lo = prevX, hi = x;
			for (int k = 0; k < 80; ++k) {
				const double mid = 0.5 * (lo + hi);
				if (diff(lo) * diff(mid) <= 0.0) hi = mid;
				else lo = mid;
			}
			cuts.push_back(0.5 * (lo + hi));
			r.crossings.push_back(cuts.back());
		}
		prevX = x;
		prev = cur;
	}
	cuts.push_back(b);

	const std::string h = wrap(f) + "-" + wrap(g);
	r.steps.push_back("Povrsina izmedu krivulja y = " + f + " i y = " + g + " je ∫_a^b |f(x) - g(x)| dx. "
		"Moramo znati gdje se krivulje sijeku jer se tada mijenja koja je gornja.");
	if (r.crossings.empty()) r.steps.push_back("Na [" + formatValue(a) + ", " + formatValue(b) + "] nema sjecista pa je jedna krivulja stalno iznad druge.");
	else {
		std::string xs;
		for (double c : r.crossings) xs += (xs.empty() ? "" : ", ") + formatValue(differential_calculus::snapToNice(c));
		r.steps.push_back("Sjecista (f = g) nalazimo promjenom predznaka razlike i bisekcijom: x = " + xs + ".");
	}
	for (std::size_t i = 0; i + 1 < cuts.size(); ++i) {
		const double left = cuts[i], right = cuts[i + 1];
		if (right - left < 1e-12) continue;
		const auto piece = integrate(h, left, right);
		const double v = std::fabs(piece.value);
		r.area += v;
		r.steps.push_back("Dio [" + formatValue(left) + ", " + formatValue(right) + "]: |∫ (f-g) dx| = " + formatValue(v) + ".");
	}
	r.steps.push_back("Ukupna povrsina = " + formatValue(r.area) + ".");
	return r;
}

ImproperIntegralInfo DefiniteIntegration::improper(const std::string& expression, double a, bool lowerInfinite,
	double b, bool upperInfinite) {
	if (!lowerInfinite && !upperInfinite) throw std::invalid_argument("Barem jedna granica mora biti beskonacna.");
	if (lowerInfinite && upperInfinite) {
		auto left = improper(expression, 0.0, true, 0.0, false);
		auto right = improper(expression, 0.0, false, 0.0, true);
		ImproperIntegralInfo r = right;
		r.lowerInfinite = true;
		r.upperInfinite = true;
		r.expression = left.expression;
		if (left.kind == ImproperKind::Convergent && right.kind == ImproperKind::Convergent) {
			r.kind = ImproperKind::Convergent;
			r.value = left.value + right.value;
		} else {
			r.kind = (left.kind == ImproperKind::Divergent || right.kind == ImproperKind::Divergent)
				? ImproperKind::Divergent : ImproperKind::Unknown;
			r.value = 0.0;
		}
		r.steps.clear();
		r.steps.push_back("Integral na (-inf, +inf) rastavimo u tocki 0: ∫_{-inf}^{inf} = ∫_{-inf}^0 + ∫_0^{inf}. "
			"Konvergira samo ako konvergiraju oba dijela.");
		r.steps.insert(r.steps.end(), left.steps.begin(), left.steps.end());
		r.steps.insert(r.steps.end(), right.steps.begin(), right.steps.end());
		r.steps.push_back(r.kind == ImproperKind::Convergent ? "Oba dijela konvergiraju, zbroj je " + formatValue(r.value) + "."
															  : "Barem jedan dio ne konvergira pa integral divergira (ili se ne moze odluciti).");
		return r;
	}

	ImproperIntegralInfo r;
	r.expression = differential_calculus::toString(differential_calculus::simplify(parseFunction(expression)));
	r.lower = a;
	r.upper = b;
	r.lowerInfinite = lowerInfinite;
	r.upperInfinite = upperInfinite;
	const double finite = lowerInfinite ? b : a;
	r.steps.push_back(std::string("Nepravi integral definiramo limesom: ") +
		(upperInfinite ? "∫_a^{inf} f dx = lim_{t->inf} ∫_a^t f dx." : "∫_{-inf}^b f dx = lim_{t->-inf} ∫_t^b f dx.") +
		" Racunamo integral do sve udaljenijih granica t i promatramo ponasanje.");

	for (int k = 1; k <= 6; ++k) {
		const double t = std::pow(10.0, k);
		const double bound = upperInfinite ? finite + t : finite - t;
		const double wide = wideIntegral(expression, finite, upperInfinite ? t : -t);
		const double v = lowerInfinite ? -wide : wide;
		r.bounds.push_back(bound);
		r.partials.push_back(v);
	}
	const std::size_t n = r.partials.size();
	const double d1 = std::fabs(r.partials[n - 1] - r.partials[n - 2]);
	const double d0 = std::fabs(r.partials[n - 2] - r.partials[n - 3]);
	const double last = r.partials[n - 1];
	if (!std::isfinite(last)) {
		r.kind = ImproperKind::Divergent;
	} else if (d1 <= 1e-5 * (1.0 + std::fabs(last)) && d1 <= d0) {
		r.kind = ImproperKind::Convergent;
		r.value = differential_calculus::snapToNice(last);
	} else if (std::fabs(last) > 1e5 || d1 > d0 * 0.5) {
		r.kind = ImproperKind::Divergent;
	} else {
		r.kind = ImproperKind::Unknown;
	}
	std::string seq;
	for (std::size_t i = 0; i < r.partials.size(); ++i)
		seq += (i ? ", " : "") + std::string("t=") + formatValue(r.bounds[i]) + ": " + formatValue(r.partials[i]);
	r.steps.push_back("Vrijednosti integrala za rastuce |t|: " + seq + ".");
	if (r.kind == ImproperKind::Convergent) r.steps.push_back("Vrijednosti se stabiliziraju, integral konvergira i iznosi priblizno " + formatValue(r.value) + ".");
	else if (r.kind == ImproperKind::Divergent) r.steps.push_back("Vrijednosti neograniceno rastu (ili ne stabiliziraju), integral divergira.");
	else r.steps.push_back("Numericki se ne moze pouzdano odluciti; potrebna je analiticka usporedba (npr. s 1/x^p).");
	return r;
}

DefiniteIntegralResult DefiniteIntegration::meanValue(const std::string& expression, double a, double b) {
	if (!(a < b)) throw std::invalid_argument("Potrebno je a < b.");
	DefiniteIntegralResult r = integrate(expression, a, b);
	const double mean = r.value / (b - a);
	const auto f = parseFunction(expression);
	r.steps.push_back("Srednja vrijednost: f_sr = (1/(b-a))·∫_a^b f dx = " + formatValue(mean) + ".");
	double prevX = a, prev = evaluate(f, a) - mean;
	for (int i = 1; i <= 2000; ++i) {
		const double x = a + (b - a) * i / 2000.0;
		const double cur = evaluate(f, x) - mean;
		if (std::isfinite(prev) && std::isfinite(cur) && prev * cur <= 0.0) {
			double lo = prevX, hi = x;
			for (int k = 0; k < 80; ++k) {
				const double mid = 0.5 * (lo + hi);
				if ((evaluate(f, lo) - mean) * (evaluate(f, mid) - mean) <= 0.0) hi = mid;
				else lo = mid;
			}
			r.steps.push_back("Teorem srednje vrijednosti za integrale: postoji c u [a,b] s f(c) = f_sr. Jedan takav c je " +
				formatValue(0.5 * (lo + hi)) + ".");
			return r;
		}
		prevX = x;
		prev = cur;
	}
	r.steps.push_back("Za neprekidnu f postoji c u [a,b] s f(c) = f_sr (teorem srednje vrijednosti).");
	return r;
}

} // namespace aksiomat::integral_calculus
