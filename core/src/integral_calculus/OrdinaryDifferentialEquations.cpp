#include "aksiomat/integral_calculus/OrdinaryDifferentialEquations.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/IndefiniteIntegral.hpp"
#include "aksiomat/integral_calculus/PolynomialExpr.hpp"

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::evaluate;
using differential_calculus::formatValue;
using differential_calculus::parseFunction;

std::string num(double v) { return "(" + formatValue(v) + ")"; }

void requireRange(double x0, double xEnd, int count) {
	if (!std::isfinite(x0) || !std::isfinite(xEnd) || !std::isfinite(count))
		throw std::invalid_argument("Vrijednosti moraju biti konacne.");
	if (count < 2 || count > 2000) throw std::invalid_argument("Broj tocaka mora biti izmedu 2 i 2000.");
}

// RK4 za y' = g(x)·h(y).
void integrateSeparable(const differential_calculus::FnExpr::Ptr& g, const differential_calculus::FnExpr::Ptr& h,
	double x0, double y0, double xEnd, int count, OdeSolution& out) {
	const double step = (xEnd - x0) / (count - 1);
	auto f = [&](double x, double y) { return evaluate(g, x) * evaluate(h, y); };
	double x = x0, y = y0;
	for (int i = 0; i < count; ++i) {
		out.xs.push_back(x);
		out.ys.push_back(y);
		const double k1 = f(x, y);
		const double k2 = f(x + step / 2, y + step * k1 / 2);
		const double k3 = f(x + step / 2, y + step * k2 / 2);
		const double k4 = f(x + step, y + step * k3);
		y += step * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
		x += step;
		if (!std::isfinite(y)) break;
	}
}

} // namespace

OdeSolution OrdinaryDifferentialEquations::separable(const std::string& g, const std::string& h, bool hasInitial,
	double x0, double y0, double xEnd, int count) {
	const auto ge = parseFunction(g, "x");
	const auto he = parseFunction(h, "y");
	if (hasInitial) requireRange(x0, xEnd, count);

	OdeSolution r;
	r.type = "separable";
	r.equation = "y' = (" + g + ") · (" + h + ")";
	r.hasInitial = hasInitial;
	r.x0 = x0;
	r.y0 = y0;
	r.steps.push_back("Jednadzba je oblika y' = g(x)·h(y), pa se varijable razdvajaju: dy/h(y) = g(x) dx. "
		"Pretpostavljamo h(y) != 0; konstantna rjesenja h(y) = 0 treba posebno provjeriti.");

	const auto leftInt = IndefiniteIntegration::integrate("1/(" + h + ")", "y");
	const auto rightInt = IndefiniteIntegration::integrate(g, "x");
	if (!leftInt.found || !rightInt.found)
		throw std::invalid_argument("Integral nakon razdvajanja varijabli nije pronaden podrzanim metodama.");

	const std::string H = leftInt.antiderivative;
	const std::string G = rightInt.antiderivative;
	r.steps.push_back("Integriramo obje strane: ∫ dy/(" + h + ") = ∫ (" + g + ") dx.");
	r.steps.push_back("Lijeva strana: H(y) = " + H + ". Desna strana: G(x) = " + G + ".");
	r.general = H + " = " + G + " + C";
	r.steps.push_back("Opce rjesenje (implicitno): " + r.general + ".");

	const auto poly = toPolynomial(he);
	if (poly && poly->size() == 2 && std::fabs((*poly)[0]) < 1e-12 && std::fabs((*poly)[1]) > 1e-12) {
		const double a = (*poly)[1];
		r.explicitForm = true;
		r.general = "y = C·exp(" + num(a) + "·(" + G + "))";
		r.steps.push_back("Posebno, h(y) = " + formatValue(a) + "·y, pa se rjesenje moze izraziti eksplicitno: y = C·e^(" +
			formatValue(a) + "·G(x)).");
	}

	if (!hasInitial) {
		r.verification = "Za provjeru zadajte pocetni uvjet y(x0) = y0.";
		return r;
	}

	const auto He = parseFunction(H, "y");
	const auto Ge = parseFunction(G, "x");
	const double c = evaluate(He, y0) - evaluate(Ge, x0);
	if (!std::isfinite(c)) throw std::invalid_argument("Pocetni uvjet nije u domeni rjesenja.");
	r.constant = c;
	r.particular = H + " = " + G + " + " + num(c);
	r.steps.push_back("Cauchyjev problem: uvrstimo y(" + formatValue(x0) + ") = " + formatValue(y0) +
		" u H(y) = G(x) + C pa je C = H(y0) - G(x0) = " + formatValue(c) + ".");
	if (r.explicitForm) {
		const double a = (*poly)[1];
		const double C = y0 / std::exp(a * evaluate(Ge, x0));
		r.constant = C;
		r.particular = "y = " + num(C) + "·exp(" + num(a) + "·(" + G + "))";
		r.steps.push_back("U eksplicitnom obliku je C = y0/e^(a·G(x0)) = " + formatValue(C) + ".");
	}
	r.steps.push_back("Partikularno rjesenje: " + r.particular + ".");

	integrateSeparable(ge, he, x0, y0, xEnd, count, r);
	double worst = 0.0;
	for (std::size_t i = 0; i < r.xs.size(); ++i) {
		const double lhs = evaluate(He, r.ys[i]) - evaluate(Ge, r.xs[i]);
		if (std::isfinite(lhs)) worst = std::max(worst, std::fabs(lhs - c));
	}
	r.residual = worst;
	r.verified = worst < 1e-4 * (1.0 + std::fabs(c));
	r.verification = "Numericka provjera (RK4): H(y) - G(x) ostaje konstantan uz najvece odstupanje " + formatValue(worst) +
		(r.verified ? ", rjesenje je potvrdeno." : ", odstupanje je preveliko; rjesenje vrijedi samo lokalno.");
	r.steps.push_back(r.verification);
	return r;
}

OdeSolution OrdinaryDifferentialEquations::linearFirstOrder(const std::string& p, const std::string& q, bool hasInitial,
	double x0, double y0, double xEnd, int count) {
	const auto pe = parseFunction(p, "x");
	const auto qe = parseFunction(q, "x");
	if (hasInitial) requireRange(x0, xEnd, count);

	OdeSolution r;
	r.type = "linear";
	r.equation = "y' + (" + p + ")·y = " + q;
	r.hasInitial = hasInitial;
	r.x0 = x0;
	r.y0 = y0;
	r.explicitForm = true;
	r.steps.push_back("Linearna jednadzba 1. reda y' + p(x)·y = q(x). Mnozimo je integrirajucim faktorom mu(x) = e^(∫p dx) "
		"da lijeva strana postane (mu·y)'.");

	const auto P = IndefiniteIntegration::integrate(p, "x");
	if (!P.found) throw std::invalid_argument("Integral od p(x) nije pronaden.");
	std::string mu = "exp(" + P.antiderivative + ")";
	r.steps.push_back("∫ p dx = " + P.antiderivative + ", pa je mu(x) = " + mu + ".");
	if (P.antiderivative.find("ln(abs(") != std::string::npos) {
		const double k1 = evaluate(pe, 1.5) * 1.5, k2 = evaluate(pe, 4.0) * 4.0;
		if (std::isfinite(k1) && std::fabs(k1 - k2) < 1e-9 && hasInitial && x0 > 0.0 && xEnd > 0.0) {
			mu = "x^" + num(k1);
			r.steps.push_back("Za p(x) = k/x je e^(k·ln|x|) = |x|^k; na x > 0 to je x^" + formatValue(k1) + ", pa uzimamo mu(x) = " + mu + ".");
		}
	}

	const auto Q = IndefiniteIntegration::integrate(mu + "*(" + q + ")", "x");
	if (!Q.found) throw std::invalid_argument("Integral od mu(x)·q(x) nije pronaden.");
	r.steps.push_back("Tada je (mu·y)' = mu·q, pa je mu·y = ∫ mu·q dx = " + Q.antiderivative + " + C.");
	r.general = "y = (" + Q.antiderivative + " + C)/" + mu;
	r.steps.push_back("Opce rjesenje: " + r.general + ".");

	if (!hasInitial) {
		r.verification = "Za provjeru zadajte pocetni uvjet y(x0) = y0.";
		return r;
	}

	const auto mue = parseFunction(mu, "x");
	const auto Qe = parseFunction(Q.antiderivative, "x");
	const double c = y0 * evaluate(mue, x0) - evaluate(Qe, x0);
	if (!std::isfinite(c)) throw std::invalid_argument("Pocetni uvjet nije u domeni rjesenja.");
	r.constant = c;
	r.particular = "y = (" + Q.antiderivative + " + " + num(c) + ")/" + mu;
	r.steps.push_back("Cauchyjev problem: iz y(" + formatValue(x0) + ") = " + formatValue(y0) + " slijedi C = y0·mu(x0) - ∫mu·q(x0) = " +
		formatValue(c) + ".");
	r.steps.push_back("Partikularno rjesenje: " + r.particular + ".");

	const auto sol = parseFunction(r.particular.substr(4), "x");
	const double step = (xEnd - x0) / (count - 1);
	double worst = 0.0;
	for (int i = 0; i < count; ++i) {
		const double x = x0 + step * i;
		const double y = evaluate(sol, x);
		r.xs.push_back(x);
		r.ys.push_back(y);
		const double hh = 1e-5 * (1.0 + std::fabs(x));
		const double dy = (evaluate(sol, x + hh) - evaluate(sol, x - hh)) / (2 * hh);
		const double res = dy + evaluate(pe, x) * y - evaluate(qe, x);
		if (std::isfinite(res)) worst = std::max(worst, std::fabs(res) / (1.0 + std::fabs(y)));
	}
	r.residual = worst;
	r.verified = worst < 1e-4;
	r.verification = "Provjera uvrstavanjem u jednadzbu: |y' + p·y - q| je najvise " + formatValue(worst) +
		(r.verified ? ", rjesenje je potvrdeno." : ", odstupanje je preveliko.");
	r.steps.push_back(r.verification);
	return r;
}

OdeSolution OrdinaryDifferentialEquations::exponentialGrowth(double k, double y0, double tEnd, int count) {
	if (!std::isfinite(k) || !std::isfinite(y0) || !std::isfinite(tEnd)) throw std::invalid_argument("Parametri moraju biti konacni.");
	auto r = separable(formatValue(k), "y", true, 0.0, y0, tEnd, count);
	r.steps.insert(r.steps.begin(), std::string("Model ") + (k >= 0 ? "rasta" : "raspada") +
		": brzina promjene proporcionalna je kolicini, y' = k·y (slozene kamate, populacija, radioaktivni raspad). "
		"Rjesenje je y = y0·e^(k·t).");
	return r;
}

OdeSolution OrdinaryDifferentialEquations::logistic(double r, double capacity, double y0, double tEnd, int count) {
	if (!(capacity > 0.0) || !std::isfinite(r) || !std::isfinite(y0) || !std::isfinite(tEnd))
		throw std::invalid_argument("Kapacitet K mora biti pozitivan, a parametri konacni.");
	auto s = separable(formatValue(r), "y*(1-y/" + formatValue(capacity) + ")", true, 0.0, y0, tEnd, count);
	s.steps.insert(s.steps.begin(), "Logisticki model y' = r·y·(1 - y/K): rast je gotovo eksponencijalan dok je y malen, "
		"a usporava kad se y priblizava kapacitetu K. Integral lijeve strane rjesava se rastavom na parcijalne razlomke.");
	return s;
}

} // namespace aksiomat::integral_calculus
