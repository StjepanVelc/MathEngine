#include "aksiomat/differential_calculus/Differentiation.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;

Ptr num(double v) { return FnExpr::number(v); }
Ptr add(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Add, std::move(a), std::move(b)); }
Ptr sub(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Subtract, std::move(a), std::move(b)); }
Ptr mul(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Multiply, std::move(a), std::move(b)); }
Ptr div(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Divide, std::move(a), std::move(b)); }
Ptr pow(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Power, std::move(a), std::move(b)); }
Ptr neg(Ptr a) { return FnExpr::unary(FnOp::Negate, std::move(a)); }
Ptr fn(FnFunc f, Ptr a) { return FnExpr::call(f, std::move(a)); }

// Derivacija vanjske funkcije f'(u) (bez faktora u').
Ptr outerDerivative(FnFunc func, const Ptr& u, std::string& formula) {
	switch (func) {
	case FnFunc::Sin: formula = "(sin u)' = cos u"; return fn(FnFunc::Cos, u);
	case FnFunc::Cos: formula = "(cos u)' = -sin u"; return neg(fn(FnFunc::Sin, u));
	case FnFunc::Tan: formula = "(tan u)' = 1/cos^2 u"; return div(num(1), pow(fn(FnFunc::Cos, u), num(2)));
	case FnFunc::Exp: formula = "(e^u)' = e^u"; return fn(FnFunc::Exp, u);
	case FnFunc::Ln: formula = "(ln u)' = 1/u"; return div(num(1), u);
	case FnFunc::Sqrt: formula = "(sqrt u)' = 1/(2 sqrt u)"; return div(num(1), mul(num(2), fn(FnFunc::Sqrt, u)));
	case FnFunc::Abs: formula = "(|u|)' = u/|u|  (u != 0)"; return div(u, fn(FnFunc::Abs, u));
	case FnFunc::Asin:
		formula = "(arcsin u)' = 1/sqrt(1 - u^2)";
		return div(num(1), fn(FnFunc::Sqrt, sub(num(1), pow(u, num(2)))));
	case FnFunc::Acos:
		formula = "(arccos u)' = -1/sqrt(1 - u^2)";
		return neg(div(num(1), fn(FnFunc::Sqrt, sub(num(1), pow(u, num(2))))));
	case FnFunc::Atan: formula = "(arctan u)' = 1/(1 + u^2)"; return div(num(1), add(num(1), pow(u, num(2))));
	case FnFunc::Sinh: formula = "(sinh u)' = cosh u"; return fn(FnFunc::Cosh, u);
	case FnFunc::Cosh: formula = "(cosh u)' = sinh u"; return fn(FnFunc::Sinh, u);
	case FnFunc::Tanh: formula = "(tanh u)' = 1/cosh^2 u"; return div(num(1), pow(fn(FnFunc::Cosh, u), num(2)));
	}
	throw std::invalid_argument("Nepodrzana funkcija");
}

void note(std::vector<std::string>* steps, const Ptr& e, const std::string& text) {
	if (steps) steps->push_back("(" + toString(e) + ")'  ->  " + text);
}

Ptr deriveImpl(const Ptr& e, std::vector<std::string>* steps) {
	switch (e->op()) {
	case FnOp::Number:
		note(steps, e, "derivacija konstante je 0");
		return num(0);
	case FnOp::Variable:
		note(steps, e, "derivacija varijable je 1");
		return num(1);
	case FnOp::Negate:
		note(steps, e, "konstantni faktor -1: (-f)' = -f'");
		return neg(deriveImpl(e->lhs(), steps));
	case FnOp::Add:
		note(steps, e, "pravilo zbroja: (f + g)' = f' + g'");
		return add(deriveImpl(e->lhs(), steps), deriveImpl(e->rhs(), steps));
	case FnOp::Subtract:
		note(steps, e, "pravilo razlike: (f - g)' = f' - g'");
		return sub(deriveImpl(e->lhs(), steps), deriveImpl(e->rhs(), steps));
	case FnOp::Multiply: {
		const bool lhsConstant = !dependsOnVariable(e->lhs());
		const bool rhsConstant = !dependsOnVariable(e->rhs());
		if (lhsConstant && rhsConstant) {
			note(steps, e, "cijeli izraz je konstanta, derivacija je 0");
			return num(0);
		}
		if (lhsConstant) {
			note(steps, e, "konstantni faktor: (c*f)' = c*f'");
			return mul(e->lhs(), deriveImpl(e->rhs(), steps));
		}
		if (rhsConstant) {
			note(steps, e, "konstantni faktor: (f*c)' = c*f'");
			return mul(e->rhs(), deriveImpl(e->lhs(), steps));
		}
		note(steps, e, "pravilo produkta: (f*g)' = f'*g + f*g'");
		return add(mul(deriveImpl(e->lhs(), steps), e->rhs()), mul(e->lhs(), deriveImpl(e->rhs(), steps)));
	}
	case FnOp::Divide: {
		if (!dependsOnVariable(e->rhs())) {
			if (!dependsOnVariable(e->lhs())) {
				note(steps, e, "cijeli izraz je konstanta, derivacija je 0");
				return num(0);
			}
			note(steps, e, "dijeljenje konstantom: (f/c)' = f'/c");
			return div(deriveImpl(e->lhs(), steps), e->rhs());
		}
		note(steps, e, "pravilo kvocijenta: (f/g)' = (f'*g - f*g')/g^2");
		Ptr numerator = sub(mul(deriveImpl(e->lhs(), steps), e->rhs()), mul(e->lhs(), deriveImpl(e->rhs(), steps)));
		return div(numerator, pow(e->rhs(), num(2)));
	}
	case FnOp::Power: {
		const Ptr& base = e->lhs();
		const Ptr& exponent = e->rhs();
		const bool baseVaries = dependsOnVariable(base);
		const bool exponentVaries = dependsOnVariable(exponent);
		if (!baseVaries && !exponentVaries) {
			note(steps, e, "cijeli izraz je konstanta, derivacija je 0");
			return num(0);
		}
		if (!exponentVaries) {
			note(steps, e, "pravilo potencije + lancano pravilo: (u^n)' = n*u^(n-1)*u'");
			Ptr newExponent = exponent->op() == FnOp::Number ? num(exponent->value() - 1.0) : sub(exponent, num(1));
			return mul(mul(exponent, pow(base, newExponent)), deriveImpl(base, steps));
		}
		if (!baseVaries) {
			note(steps, e, "eksponencijalna funkcija: (a^u)' = a^u*ln(a)*u'");
			return mul(mul(e, fn(FnFunc::Ln, base)), deriveImpl(exponent, steps));
		}
		note(steps, e, "opce pravilo: (u^v)' = u^v*(v'*ln(u) + v*u'/u)");
		Ptr inner = add(mul(deriveImpl(exponent, steps), fn(FnFunc::Ln, base)),
			div(mul(exponent, deriveImpl(base, steps)), base));
		return mul(e, inner);
	}
	case FnOp::Call: {
		std::string formula;
		Ptr outer = outerDerivative(e->func(), e->lhs(), formula);
		note(steps, e, "lancano pravilo: " + formula + ", pa je (f(u))' = f'(u)*u'");
		if (e->lhs()->op() == FnOp::Variable) return outer;
		return mul(outer, deriveImpl(e->lhs(), steps));
	}
	}
	throw std::invalid_argument("Nepodrzan izraz");
}

} // namespace

FnExpr::Ptr Differentiation::derive(const FnExpr::Ptr& expression, std::vector<std::string>* steps) {
	if (!expression) throw std::invalid_argument("Prazan izraz");
	return deriveImpl(expression, steps);
}

DerivativeResult Differentiation::differentiate(const std::string& expression, int order,
	const std::string& variable) {
	if (order < 1 || order > 10) throw std::invalid_argument("Red derivacije mora biti izmedju 1 i 10");
	DerivativeResult result;
	result.expression = expression;
	result.order = order;

	Ptr current = simplify(parseFunction(expression, variable));
	result.expression = toString(current);
	for (int k = 1; k <= order; ++k) {
		if (order > 1) {
			result.steps.push_back("Korak " + std::to_string(k) + ": deriviramo f" +
				(k > 1 ? "^(" + std::to_string(k - 1) + ")" : std::string()) + "(x) = " + toString(current));
		}
		Ptr raw = deriveImpl(current, &result.steps);
		Ptr simple = simplify(raw);
		result.steps.push_back("Spajanje i pojednostavljenje: " + toString(raw) + "  =  " + toString(simple));
		if (k == order) result.rawDerivative = toString(raw);
		current = simple;
	}
	result.derivative = toString(current);
	return result;
}

double Differentiation::derivativeAt(const std::string& expression, double point, int order,
	const std::string& variable) {
	if (order < 0 || order > 10) throw std::invalid_argument("Red derivacije mora biti izmedju 0 i 10");
	Ptr current = simplify(parseFunction(expression, variable));
	for (int k = 0; k < order; ++k) current = simplify(deriveImpl(current, nullptr));
	return evaluate(current, point);
}

bool equivalentFunctions(const std::string& first, const std::string& second, const std::string& variable) {
	const Ptr a = parseFunction(first, variable);
	const Ptr b = parseFunction(second, variable);
	static const double points[] = {-3.7, -2.3, -1.1, -0.4, 0.3, 0.9, 1.7, 2.6, 3.4, 5.1, 0.55, -0.85};
	int compared = 0;
	for (const double x : points) {
		const double va = evaluate(a, x);
		const double vb = evaluate(b, x);
		if (!std::isfinite(va) || !std::isfinite(vb)) continue;
		++compared;
		if (std::fabs(va - vb) > 1e-7 * (1.0 + std::fabs(va))) return false;
	}
	return compared >= 3;
}

} // namespace aksiomat::differential_calculus
