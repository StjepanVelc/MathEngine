#include <cmath>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/IntegrationInternal.hpp"

namespace aksiomat::integral_calculus::detail {
namespace {

using differential_calculus::evaluate;
using differential_calculus::toString;

constexpr double kTiny = 1e-12;

bool isInteger(double v) { return std::fabs(v - std::round(v)) < 1e-9; }

Ptr over(Ptr e, double a) { return divideByConstant(std::move(e), a); }

Outcome make(Ptr F, std::vector<std::string> steps) {
	Attempt a;
	a.antiderivative = std::move(F);
	a.steps = std::move(steps);
	a.method = "tablicna";
	return a;
}

Ptr sqrtOf(double value) {
	const double r = std::sqrt(value);
	if (isInteger(r)) return num(std::round(r));
	return call(FnFunc::Sqrt, num(value));
}

Ptr inner(int power, FnFunc f, const Ptr& u) {
	if (power == 0) return num(1.0);
	if (power == 1) return call(f, u);
	return pow(call(f, u), num(power));
}

Outcome functionRule(const Ptr& f, FnFunc func, const Ptr& u, const Linear& lin, const std::string& v) {
	const std::string us = show(u);
	const double a = lin.a;
	const std::string chain = (std::fabs(a - 1.0) < kTiny)
		? std::string()
		: " Argument je linearan (u = " + us + ", u' = " + differential_calculus::formatValue(a) +
			"), pa rezultat dijelimo s u'.";
	switch (func) {
	case FnFunc::Sin:
		return make(over(neg(call(FnFunc::Cos, u)), a), {"Tablica: ∫ sin(u) du = -cos(u)." + chain});
	case FnFunc::Cos:
		return make(over(call(FnFunc::Sin, u), a), {"Tablica: ∫ cos(u) du = sin(u)." + chain});
	case FnFunc::Exp:
		return make(over(f, a), {"Tablica: ∫ e^u du = e^u." + chain});
	case FnFunc::Tan:
		return make(over(neg(lnAbs(call(FnFunc::Cos, u))), a), {"Tablica: ∫ tan(u) du = -ln|cos(u)|." + chain});
	case FnFunc::Sinh:
		return make(over(call(FnFunc::Cosh, u), a), {"Tablica: ∫ sinh(u) du = cosh(u)." + chain});
	case FnFunc::Cosh:
		return make(over(call(FnFunc::Sinh, u), a), {"Tablica: ∫ cosh(u) du = sinh(u)." + chain});
	case FnFunc::Tanh:
		return make(over(call(FnFunc::Ln, call(FnFunc::Cosh, u)), a),
			{"Tablica: ∫ tanh(u) du = ln(cosh(u))." + chain});
	case FnFunc::Ln:
		return make(over(sub(mul(u, lnAbs(u)), u), a),
			{"Parcijalnom integracijom (u = ln u, dv = du): ∫ ln(u) du = u·ln|u| - u." + chain});
	case FnFunc::Asin:
		return make(over(add(mul(u, call(FnFunc::Asin, u)), call(FnFunc::Sqrt, sub(num(1.0), pow(u, num(2.0))))), a),
			{"Parcijalno: ∫ arcsin(u) du = u·arcsin(u) + sqrt(1-u^2)." + chain});
	case FnFunc::Acos:
		return make(over(sub(mul(u, call(FnFunc::Acos, u)), call(FnFunc::Sqrt, sub(num(1.0), pow(u, num(2.0))))), a),
			{"Parcijalno: ∫ arccos(u) du = u·arccos(u) - sqrt(1-u^2)." + chain});
	case FnFunc::Atan:
		return make(over(sub(mul(u, call(FnFunc::Atan, u)),
							 div(call(FnFunc::Ln, add(num(1.0), pow(u, num(2.0)))), num(2.0))), a),
			{"Parcijalno: ∫ arctan(u) du = u·arctan(u) - (1/2)·ln(1+u^2)." + chain});
	default: break;
	}
	(void)v;
	return std::nullopt;
}

Outcome powerRule(const Ptr& base, double n, const std::string& v, int depth);

Outcome trigPower(FnFunc func, const Ptr& u, const Linear& lin, int n, const std::string& v, int depth) {
	const double a = lin.a;
	const bool isSin = func == FnFunc::Sin;
	if (n == -2) {
		if (isSin)
			return make(over(neg(div(call(FnFunc::Cos, u), call(FnFunc::Sin, u))), a),
				{"Tablica: ∫ 1/sin^2(u) du = -ctg(u)."});
		return make(over(call(FnFunc::Tan, u), a), {"Tablica: ∫ 1/cos^2(u) du = tg(u)."});
	}
	if (n < 2 || n > 12) return std::nullopt;
	const auto rest = integrate(inner(n - 2, func, u), v, depth + 1);
	if (!rest) return std::nullopt;
	const Ptr head = isSin
		? neg(div(mul(pow(call(FnFunc::Sin, u), num(n - 1)), call(FnFunc::Cos, u)), num(n)))
		: div(mul(pow(call(FnFunc::Cos, u), num(n - 1)), call(FnFunc::Sin, u)), num(n));
	const Ptr tail = mul(div(num(n - 1), num(n)), rest->antiderivative);
	std::vector<std::string> steps{std::string("Redukcijska formula: ∫ ") + (isSin ? "sin" : "cos") +
		"^n(u) du = " + (isSin ? "-" : "") + (isSin ? "sin" : "cos") + "^(n-1)(u)·" + (isSin ? "cos" : "sin") +
		"(u)/n + ((n-1)/n)·∫ " + (isSin ? "sin" : "cos") + "^(n-2)(u) du, ovdje n = " + std::to_string(n) + "."};
	for (const auto& s : rest->steps) steps.push_back(s);
	return make(over(add(head, tail), a), steps);
}

Outcome quadraticRule(const Ptr& base, double n, const std::string& v) {
	const auto poly = toPolynomial(base);
	if (!poly || polynomialDegree(*poly) != 2 || std::fabs(n + 1.0) > kTiny) return std::nullopt;
	const double c = (*poly)[0], b = (*poly)[1], a = (*poly)[2];
	const double delta = 4.0 * a * c - b * b;
	if (std::fabs(delta) < 1e-10) {
		return make(div(num(-2.0), fromPolynomial({b, 2.0 * a}, v)),
			{"Nazivnik je potpuni kvadrat a(x + b/(2a))^2, pa je ∫ dx/(...) = -1/(a·(x + b/(2a))) = -2/(2ax + b)."});
	}
	if (delta < 0.0) return std::nullopt;
	const double s = std::sqrt(delta);
	const Ptr arg = fromPolynomial({b / s, 2.0 * a / s}, v);
	return make(scaleBy(2.0 / s, call(FnFunc::Atan, arg)),
		{"Kvadratni nazivnik bez realnih nultocaka (D < 0): nadopunimo na potpuni kvadrat i dobijemo arctg. "
		 "Formula: ∫ dx/(ax^2+bx+c) = 2/sqrt(4ac-b^2) · arctg((2ax+b)/sqrt(4ac-b^2))."});
}

Outcome sqrtQuadraticRule(const Ptr& quadratic, double n, const std::string& v) {
	const auto poly = toPolynomial(quadratic);
	if (!poly || polynomialDegree(*poly) != 2 || std::fabs((*poly)[1]) > kTiny) return std::nullopt;
	const double c0 = (*poly)[0], c2 = (*poly)[2];
	if (std::fabs(c0) < kTiny) return std::nullopt;
	const Ptr x = var(v);
	const Ptr root = call(FnFunc::Sqrt, quadratic);
	if (std::fabs(n + 1.0) < kTiny) {
		if (c2 < 0.0 && c0 > 0.0) {
			const double k = std::sqrt(-c2);
			return make(over(call(FnFunc::Asin, mul(num(std::sqrt(-c2 / c0)), x)), k),
				{"Oblik ∫ dx/sqrt(c - k·x^2) = (1/sqrt(k))·arcsin(x·sqrt(k/c))."});
		}
		if (c2 > 0.0) {
			const double k = std::sqrt(c2);
			return make(over(lnAbs(add(mul(num(k), x), root)), k),
				{"Oblik ∫ dx/sqrt(k·x^2 + c) = (1/sqrt(k))·ln|sqrt(k)·x + sqrt(k·x^2 + c)|."});
		}
		return std::nullopt;
	}
	if (std::fabs(n - 1.0) < kTiny) {
		const Ptr head = div(mul(x, root), num(2.0));
		if (c2 < 0.0 && c0 > 0.0) {
			const double k = std::sqrt(-c2);
			const Ptr tail = scaleBy(c0 / (2.0 * k), call(FnFunc::Asin, mul(num(std::sqrt(-c2 / c0)), x)));
			return make(add(head, tail), {"Oblik ∫ sqrt(c - k·x^2) dx = x·sqrt(...)/2 + (c/(2·sqrt(k)))·arcsin(x·sqrt(k/c))."});
		}
		if (c2 > 0.0) {
			const double k = std::sqrt(c2);
			const Ptr tail = scaleBy(c0 / (2.0 * k), lnAbs(add(mul(num(k), x), root)));
			return make(add(head, tail), {"Oblik ∫ sqrt(k·x^2 + c) dx = x·sqrt(...)/2 + (c/(2·sqrt(k)))·ln|sqrt(k)·x + sqrt(...)|."});
		}
	}
	return std::nullopt;
}

Outcome powerRule(const Ptr& base, double n, const std::string& v, int depth) {
	if (const auto lin = asLinear(base, v)) {
		const double a = lin->a;
		if (std::fabs(n + 1.0) < kTiny) {
			return make(over(lnAbs(base), a), {"Tablica: ∫ du/u = ln|u|, gdje je u = " + show(base) +
				(std::fabs(a - 1.0) < kTiny ? "." : "; dijelimo s u' = " + differential_calculus::formatValue(a) + ".")});
		}
		return make(over(pow(base, num(n + 1.0)), (n + 1.0) * a),
			{"Pravilo potencije: ∫ u^n du = u^(n+1)/(n+1), ovdje n = " + differential_calculus::formatValue(n) + ", u = " +
				show(base) + (std::fabs(a - 1.0) < kTiny ? "." : "; dijelimo s u' = " + differential_calculus::formatValue(a) + ".")});
	}
	if (base->op() == FnOp::Call && base->func() == FnFunc::Sqrt) {
		if (const auto lin = asLinear(base->lhs(), v)) {
			(void)lin;
			return powerRule(base->lhs(), n / 2.0, v, depth);
		}
		if (const auto r = sqrtQuadraticRule(base->lhs(), n, v)) return r;
	}
	if (base->op() == FnOp::Call && isInteger(n)) {
		const FnFunc func = base->func();
		if (func == FnFunc::Sin || func == FnFunc::Cos) {
			if (const auto lin = asLinear(base->lhs(), v))
				return trigPower(func, base->lhs(), *lin, static_cast<int>(std::lround(n)), v, depth);
		}
	}
	return quadraticRule(base, n, v);
}

Outcome exponentialRule(const Ptr& f, const Ptr& base, const Ptr& exponent, const std::string& v) {
	if (containsVar(base, v)) return std::nullopt;
	const auto lin = asLinear(exponent, v);
	if (!lin) return std::nullopt;
	const double c = evaluate(base, 0.0);
	if (!(c > 0.0) || std::fabs(c - 1.0) < kTiny) return std::nullopt;
	return make(div(f, mul(num(lin->a), call(FnFunc::Ln, base))),
		{"Eksponencijalna funkcija: ∫ c^u du = c^u/ln(c), c = " + show(base) + "."});
}

Outcome expTrig(const Ptr& expPart, const Ptr& trigPart, const std::string& v) {
	const auto e = asLinear(expPart->lhs(), v);
	const auto w = asLinear(trigPart->lhs(), v);
	if (!e || !w) return std::nullopt;
	const double al = e->a, ga = w->a, den = al * al + ga * ga;
	const Ptr s = call(FnFunc::Sin, trigPart->lhs());
	const Ptr c = call(FnFunc::Cos, trigPart->lhs());
	const Ptr bracket = trigPart->func() == FnFunc::Sin ? sub(scaleBy(al, s), scaleBy(ga, c))
														 : add(scaleBy(al, c), scaleBy(ga, s));
	return make(div(mul(expPart, bracket), num(den)),
		{"Dvostruka parcijalna integracija (ciklicki slucaj): integral se vrati sam u sebe pa se rijesi "
		 "kao jednadzba. Formula: ∫ e^(αx) sin(βx) dx = e^(αx)(α sin βx - β cos βx)/(α²+β²), "
		 "odnosno ∫ e^(αx) cos(βx) dx = e^(αx)(α cos βx + β sin βx)/(α²+β²)."});
}

} // namespace

Outcome tableIntegral(const Ptr& f, const std::string& v, int depth) {
	const Factors fl = flattenFactors(f, v);
	if (std::fabs(fl.coefficient - 1.0) > kTiny) return std::nullopt;

	if (fl.numerators.size() == 2 && fl.denominators.empty()) {
		for (int swap = 0; swap < 2; ++swap) {
			const Ptr& e = fl.numerators[swap];
			const Ptr& t = fl.numerators[1 - swap];
			if (e->op() == FnOp::Call && e->func() == FnFunc::Exp && t->op() == FnOp::Call &&
				(t->func() == FnFunc::Sin || t->func() == FnFunc::Cos)) {
				if (const auto r = expTrig(e, t, v)) return r;
			}
		}
		return std::nullopt;
	}

	Ptr base;
	double exponent = 1.0;
	Ptr whole;
	if (fl.numerators.size() == 1 && fl.denominators.empty()) {
		whole = fl.numerators[0];
	} else if (fl.numerators.empty() && fl.denominators.size() == 1) {
		whole = fl.denominators[0];
		exponent = -1.0;
	} else {
		return std::nullopt;
	}

	if (whole->op() == FnOp::Power) {
		if (containsVar(whole->rhs(), v)) {
			if (exponent != 1.0) return std::nullopt;
			return exponentialRule(whole, whole->lhs(), whole->rhs(), v);
		}
		base = whole->lhs();
		exponent *= evaluate(whole->rhs(), 0.0);
	} else {
		base = whole;
	}

	if (exponent == 1.0 && base->op() == FnOp::Call) {
		if (const auto lin = asLinear(base->lhs(), v)) {
			if (base->func() == FnFunc::Sqrt) return powerRule(base->lhs(), 0.5, v, depth);
			if (const auto r = functionRule(base, base->func(), base->lhs(), *lin, v)) return r;
		}
	}
	if (base->op() == FnOp::Call && base->func() == FnFunc::Exp && exponent == 1.0) return std::nullopt;
	return powerRule(base, exponent, v, depth);
}

} // namespace aksiomat::integral_calculus::detail
