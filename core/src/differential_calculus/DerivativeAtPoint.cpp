#include "aksiomat/differential_calculus/DerivativeAtPoint.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/differential_calculus/RootFinding.hpp"

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;

std::string lineText(double k, double l) {
	k = snapToNice(k);
	l = snapToNice(l);
	std::string text = "y = ";
	if (k == 0.0) return text + formatValue(l);
	if (k == 1.0) text += "x";
	else if (k == -1.0) text += "-x";
	else text += formatValue(k) + "*x";
	if (l > 0.0) text += " + " + formatValue(l);
	else if (l < 0.0) text += " - " + formatValue(-l);
	return text;
}

void collectAbsArguments(const Ptr& e, std::vector<Ptr>& out) {
	if (!e) return;
	if (e->op() == FnOp::Call && e->func() == FnFunc::Abs) out.push_back(e->lhs());
	collectAbsArguments(e->lhs(), out);
	collectAbsArguments(e->rhs(), out);
}

} // namespace

TangentResult DerivativeAtPoint::analyze(const std::string& expression, double point, const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka mora biti konacan broj");
	const Ptr f = simplify(parseFunction(expression, variable));
	const Ptr d = simplify(Differentiation::derive(f));

	TangentResult result;
	result.expression = toString(f);
	result.derivativeExpression = toString(d);
	result.point = point;
	result.value = evaluate(f, point);
	result.valueDefined = std::isfinite(result.value);
	const std::string a = formatValue(point);

	result.steps.push_back("Derivacija u tocki a po definiciji: f'(a) = lim (h -> 0) [f(a + h) - f(a)] / h. "
		"Izraz [f(a + h) - f(a)] / h je nagib sekante kroz tocke (a, f(a)) i (a + h, f(a + h)); kad h -> 0 sekanta prelazi u tangentu.");
	if (!result.valueDefined) {
		result.problem = "undefined";
		result.steps.push_back("f(" + a + ") nije definiran, pa derivacija u toj tocki ne postoji.");
		return result;
	}
	result.steps.push_back("f(" + a + ") = " + formatValue(result.value) + ".");

	for (const double h : {1.0, 0.5, 0.1, 0.01, 0.001, 0.0001}) {
		const double xr = point + h;
		const double yr = evaluate(f, xr);
		result.rightRows.push_back({h, xr, (yr - result.value) / h, std::isfinite(yr)});
		const double xl = point - h;
		const double yl = evaluate(f, xl);
		result.leftRows.push_back({-h, xl, (result.value - yl) / h, std::isfinite(yl)});
	}

	constexpr double step = 1e-5;
	const double rightDerivative = (-3.0 * result.value + 4.0 * evaluate(f, point + step) - evaluate(f, point + 2 * step)) / (2 * step);
	const double leftDerivative = (3.0 * result.value - 4.0 * evaluate(f, point - step) + evaluate(f, point - 2 * step)) / (2 * step);
	result.rightDerivative = snapToNice(rightDerivative);
	result.leftDerivative = snapToNice(leftDerivative);

	const bool rightOk = std::isfinite(rightDerivative);
	const bool leftOk = std::isfinite(leftDerivative);
	result.steps.push_back("Nagibi sekanti zdesna (h > 0) teze prema " + formatValue(result.rightDerivative) +
		", a slijeva (h < 0) prema " + formatValue(result.leftDerivative) + ".");

	if (!rightOk || !leftOk) {
		result.problem = "one_sided";
		result.steps.push_back("Funkcija nije definirana s obje strane tocke a, pa postoji najvise jednostrana derivacija.");
		return result;
	}
	if (std::fabs(rightDerivative) > 1e6 || std::fabs(leftDerivative) > 1e6) {
		result.problem = "vertical";
		result.steps.push_back("Nagibi sekanti neograniceno rastu: tangenta je okomita (x = " + a + ") i derivacija ne postoji kao konacan broj.");
		result.tangentLine = "x = " + a;
		result.normalLine = lineText(0.0, result.value);
		return result;
	}
	if (std::fabs(rightDerivative - leftDerivative) > 1e-4 * (1.0 + std::fabs(rightDerivative))) {
		result.problem = "kink";
		result.steps.push_back("Lijeva derivacija (" + formatValue(result.leftDerivative) + ") i desna derivacija (" +
			formatValue(result.rightDerivative) + ") se razlikuju: graf ima siljak (kut) u a, pa f nije derivabilna u a. "
			"Primijeti: funkcija moze biti neprekidna, a da nije derivabilna.");
		return result;
	}

	result.differentiable = true;
	const double symbolic = evaluate(d, point);
	double slope = 0.5 * (rightDerivative + leftDerivative);
	if (std::isfinite(symbolic) && std::fabs(symbolic - slope) <= 1e-3 * (1.0 + std::fabs(symbolic))) slope = symbolic;
	result.slope = snapToNice(slope);
	result.steps.push_back("Lijeva i desna derivacija se podudaraju, pa f'(" + a + ") = " + formatValue(result.slope) + ".");
	result.steps.push_back("Provjera simbolickom derivacijom: f'(x) = " + result.derivativeExpression + ", pa je f'(" + a + ") = " +
		formatValue(std::isfinite(symbolic) ? symbolic : result.slope) + ".");

	const double intercept = result.value - result.slope * point;
	result.tangentLine = lineText(result.slope, intercept);
	result.steps.push_back("Tangenta: y - f(a) = f'(a) * (x - a), tj. " + result.tangentLine + ".");
	if (result.slope == 0.0) {
		result.normalLine = "x = " + a;
		result.steps.push_back("Tangenta je vodoravna pa je normala okomita: " + result.normalLine + ".");
	} else {
		const double normalSlope = -1.0 / result.slope;
		result.normalLine = lineText(normalSlope, result.value - normalSlope * point);
		result.steps.push_back("Normala je okomita na tangentu (nagib -1/f'(a)): " + result.normalLine + ".");
	}
	return result;
}

MeanValueResult DerivativeAtPoint::meanValue(const std::string& expression, double a, double b,
	const std::string& variable) {
	if (!std::isfinite(a) || !std::isfinite(b) || !(a < b)) throw std::invalid_argument("Interval mora zadovoljavati a < b");
	const Ptr f = simplify(parseFunction(expression, variable));
	const Ptr d = simplify(Differentiation::derive(f));

	MeanValueResult result;
	result.expression = toString(f);
	result.a = a;
	result.b = b;
	result.fa = evaluate(f, a);
	result.fb = evaluate(f, b);
	if (!std::isfinite(result.fa) || !std::isfinite(result.fb)) {
		throw std::invalid_argument("Funkcija mora biti definirana u oba kraja intervala");
	}
	result.meanSlope = (result.fb - result.fa) / (b - a);

	constexpr int grid = 400;
	bool continuous = true;
	bool differentiable = true;
	for (int i = 0; i <= grid; ++i) {
		const double x = a + (b - a) * i / grid;
		const double y = evaluate(f, x);
		if (!std::isfinite(y) || std::fabs(y) > 1e8) continuous = false;
		if (i > 0 && i < grid) {
			const double slope = evaluate(d, x);
			if (!std::isfinite(slope)) differentiable = false;
		}
	}
	std::vector<Ptr> absArguments;
	collectAbsArguments(f, absArguments);
	for (const Ptr& argument : absArguments) {
		for (const double root : findRoots([&](double x) { return evaluate(argument, x); }, a, b)) {
			if (root > a + 1e-9 && root < b - 1e-9) differentiable = false;
		}
	}
	result.continuousOnInterval = continuous;
	result.differentiableInInterior = differentiable;
	result.lagrangeApplies = continuous && differentiable;
	result.rolleApplies = result.lagrangeApplies && std::fabs(result.fa - result.fb) <= 1e-9 * (1.0 + std::fabs(result.fa));

	const std::string as = formatValue(a);
	const std::string bs = formatValue(b);
	result.steps.push_back("Lagrangeov teorem: ako je f neprekidna na [a, b] i derivabilna na (a, b), postoji c iz (a, b) takav da je f'(c) = [f(b) - f(a)] / (b - a). "
		"Geometrijski: postoji tocka u kojoj je tangenta paralelna sekanti kroz krajeve grafa.");
	result.steps.push_back(std::string("Uvjet neprekidnosti na [") + as + ", " + bs + "]: " + (continuous ? "ispunjen." : "NIJE ispunjen (f ima prekid ili nije definirana)."));
	result.steps.push_back(std::string("Uvjet derivabilnosti na (") + as + ", " + bs + "): " + (differentiable ? "ispunjen." : "NIJE ispunjen (f' nije definirana ili postoji siljak)."));
	result.steps.push_back("f(" + as + ") = " + formatValue(result.fa) + ", f(" + bs + ") = " + formatValue(result.fb) +
		", nagib sekante = (" + formatValue(result.fb) + " - " + formatValue(result.fa) + ") / (" + bs + " - " + as + ") = " + formatValue(snapToNice(result.meanSlope)) + ".");
	if (result.rolleApplies) {
		result.steps.push_back("Vrijedi f(a) = f(b), pa je ovo Rolleov teorem (poseban slucaj Lagrangeovog): postoji c s f'(c) = 0, tj. vodoravnom tangentom.");
	}

	if (result.lagrangeApplies) {
		const double slope = result.meanSlope;
		for (const double root : findRoots([&](double x) { return evaluate(d, x) - slope; }, a, b)) {
			if (root > a + 1e-9 && root < b - 1e-9) {
				result.points.push_back(root);
				result.pointValues.push_back(evaluate(f, root));
			}
		}
		if (result.points.empty()) {
			result.steps.push_back("Numericki nije pronadjena tocka c (tolerancija); teorem ipak jamci njezino postojanje.");
		} else {
			std::string list;
			for (const double c : result.points) list += (list.empty() ? "" : ", ") + formatValue(snapToNice(c));
			result.steps.push_back("Rjesavamo f'(c) = " + formatValue(snapToNice(slope)) + " na (" + as + ", " + bs + "): c = " + list + ".");
		}
	} else {
		result.steps.push_back("Uvjeti teorema nisu ispunjeni, pa teorem ne jamci postojanje tocke c (ona moze, ali ne mora postojati).");
	}
	return result;
}

} // namespace aksiomat::differential_calculus
