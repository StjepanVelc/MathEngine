#include "aksiomat/integral_calculus/RiemannSums.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::FnExpr;
using differential_calculus::evaluate;
using differential_calculus::formatValue;

double sample(const FnExpr::Ptr& f, double x) {
	const double y = evaluate(f, x);
	if (!std::isfinite(y)) {
		throw std::invalid_argument("Funkcija nije definirana (ili je neogranicena) u x = " + formatValue(x) +
			", pa se integral na tom intervalu ne moze izracunati ovom metodom.");
	}
	return y;
}

double compositeSimpson(const FnExpr::Ptr& f, double a, double b, int n) {
	const double h = (b - a) / n;
	double sum = 0.0;
	for (int i = 0; i < n; ++i) {
		const double x0 = a + i * h;
		sum += h / 6.0 * (sample(f, x0) + 4.0 * sample(f, x0 + h / 2.0) + sample(f, x0 + h));
	}
	return sum;
}

} // namespace

std::string RiemannSums::methodId(RiemannMethod method) {
	switch (method) {
	case RiemannMethod::Left: return "left";
	case RiemannMethod::Right: return "right";
	case RiemannMethod::Midpoint: return "midpoint";
	case RiemannMethod::Trapezoid: return "trapezoid";
	case RiemannMethod::Simpson: return "simpson";
	}
	return "left";
}

RiemannMethod RiemannSums::parseMethod(const std::string& name) {
	if (name == "left") return RiemannMethod::Left;
	if (name == "right") return RiemannMethod::Right;
	if (name == "midpoint") return RiemannMethod::Midpoint;
	if (name == "trapezoid") return RiemannMethod::Trapezoid;
	if (name == "simpson") return RiemannMethod::Simpson;
	throw std::invalid_argument("Nepoznata metoda: " + name + " (left, right, midpoint, trapezoid, simpson)");
}

RiemannResult RiemannSums::compute(const std::string& expression, double a, double b, int n,
	RiemannMethod method, const std::string& variable) {
	if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument("Granice integrala moraju biti konacni brojevi.");
	if (!(a < b)) throw std::invalid_argument("Donja granica a mora biti manja od gornje granice b.");
	if (n < 1 || n > 5000) throw std::invalid_argument("Broj podintervala n mora biti izmedu 1 i 5000.");

	const auto f = differential_calculus::parseFunction(expression, variable);
	RiemannResult r;
	r.expression = differential_calculus::toString(f);
	r.method = methodId(method);
	r.a = a;
	r.b = b;
	r.n = n;
	r.stepWidth = (b - a) / n;
	const double h = r.stepWidth;

	for (int i = 0; i < n; ++i) {
		RiemannRect rect;
		rect.x0 = a + i * h;
		rect.x1 = (i == n - 1) ? b : a + (i + 1) * h;
		switch (method) {
		case RiemannMethod::Left: rect.height = sample(f, rect.x0); break;
		case RiemannMethod::Right: rect.height = sample(f, rect.x1); break;
		case RiemannMethod::Midpoint: rect.height = sample(f, (rect.x0 + rect.x1) / 2.0); break;
		case RiemannMethod::Trapezoid: rect.height = (sample(f, rect.x0) + sample(f, rect.x1)) / 2.0; break;
		case RiemannMethod::Simpson:
			rect.height = (sample(f, rect.x0) + 4.0 * sample(f, (rect.x0 + rect.x1) / 2.0) + sample(f, rect.x1)) / 6.0;
			break;
		}
		rect.area = rect.height * (rect.x1 - rect.x0);
		r.approximation += rect.area;
		r.rectangles.push_back(rect);

		double lo = std::numeric_limits<double>::infinity();
		double hi = -lo;
		for (int k = 0; k <= 16; ++k) {
			const double y = sample(f, rect.x0 + (rect.x1 - rect.x0) * k / 16.0);
			lo = std::min(lo, y);
			hi = std::max(hi, y);
		}
		r.lowerSum += lo * (rect.x1 - rect.x0);
		r.upperSum += hi * (rect.x1 - rect.x0);
	}

	r.reference = compositeSimpson(f, a, b, 2000);
	r.error = std::fabs(r.approximation - r.reference);

	const std::string interval = "[" + formatValue(a) + ", " + formatValue(b) + "]";
	r.steps.push_back("Odredeni integral od " + formatValue(a) + " do " + formatValue(b) + " izraza f(x) = " + r.expression +
		" je povrsina izmedu grafa i osi x (dijelovi ispod osi racunaju se negativno).");
	r.steps.push_back("Interval " + interval + " podijelimo na n = " + std::to_string(n) + " jednakih dijelova sirine "
		"Δx = (b - a)/n = " + formatValue(h) + ".");
	switch (method) {
	case RiemannMethod::Left:
		r.steps.push_back("Lijeva suma: visina svakog pravokutnika je f u lijevom rubu podintervala. S = Σ f(x_i)·Δx.");
		break;
	case RiemannMethod::Right:
		r.steps.push_back("Desna suma: visina svakog pravokutnika je f u desnom rubu podintervala. S = Σ f(x_(i+1))·Δx.");
		break;
	case RiemannMethod::Midpoint:
		r.steps.push_back("Suma srednje tocke: visina je f u sredini podintervala, pa se pogreske lijevo i desno djelomicno ponistavaju.");
		break;
	case RiemannMethod::Trapezoid:
		r.steps.push_back("Trapezno pravilo: na svakom podintervalu graf zamijenimo duzinom, povrsina trapeza je (f(x_i) + f(x_(i+1)))/2 · Δx.");
		break;
	case RiemannMethod::Simpson:
		r.steps.push_back("Simpsonovo pravilo: na svakom podintervalu graf zamijenimo parabolom kroz rubove i sredinu, "
			"povrsina je (f(x_i) + 4 f(sredina) + f(x_(i+1)))/6 · Δx.");
		break;
	}
	r.steps.push_back("Zbroj svih doprinosa daje aproksimaciju integrala: " + formatValue(r.approximation) + ".");
	r.steps.push_back("Donja suma (najmanja vrijednost f na svakom dijelu) = " + formatValue(r.lowerSum) +
		", gornja suma (najveca vrijednost) = " + formatValue(r.upperSum) +
		". Ako se za sve finije podjele ove dvije sume priblizavaju istom broju, funkcija je Riemann-integrabilna, a taj broj je integral.");
	r.steps.push_back("Za usporedbu, vrlo fina Simpsonova aproksimacija daje " + formatValue(r.reference) +
		", pa je pogreska ove aproksimacije oko " + formatValue(r.error) + ". Povecanjem n pogreska se smanjuje.");
	return r;
}

} // namespace aksiomat::integral_calculus
