#include "aksiomat/integral_calculus/IntegrationInternal.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"

namespace aksiomat::integral_calculus::detail {

using differential_calculus::evaluate;
using differential_calculus::simplify;
using differential_calculus::toString;

Ptr num(double value) { return FnExpr::number(value); }
Ptr var(const std::string& name) { return FnExpr::variable(name); }
Ptr add(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Add, std::move(a), std::move(b)); }
Ptr sub(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Subtract, std::move(a), std::move(b)); }
Ptr mul(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Multiply, std::move(a), std::move(b)); }
Ptr div(Ptr a, Ptr b) { return FnExpr::binary(FnOp::Divide, std::move(a), std::move(b)); }
Ptr neg(Ptr a) { return FnExpr::unary(FnOp::Negate, std::move(a)); }
Ptr pow(Ptr base, Ptr exponent) { return FnExpr::binary(FnOp::Power, std::move(base), std::move(exponent)); }
Ptr call(FnFunc func, Ptr argument) { return FnExpr::call(func, std::move(argument)); }
Ptr lnAbs(Ptr argument) { return call(FnFunc::Ln, call(FnFunc::Abs, std::move(argument))); }

Ptr coefficient(double value) {
	if (std::fabs(value - std::round(value)) < 1e-9) return num(std::round(value) + 0.0);
	const double magnitude = std::fabs(value);
	for (int q = 2; q <= 12; ++q) {
		const double p = std::round(magnitude * q);
		if (p > 0.0 && std::fabs(magnitude - p / q) < 1e-9) {
			Ptr fraction = div(num(p), num(q));
			return value < 0.0 ? neg(fraction) : fraction;
		}
	}
	return num(value);
}

Ptr scaleBy(double c, Ptr e) {
	if (c == 1.0) return e;
	if (c == -1.0) return neg(std::move(e));
	if (c < 0.0) return neg(mul(coefficient(-c), std::move(e)));
	return mul(coefficient(c), std::move(e));
}

Ptr divideByConstant(Ptr e, double d) {
	if (d == 1.0) return e;
	if (d == -1.0) return neg(std::move(e));
	if (d < 0.0) return neg(div(std::move(e), num(-d)));
	return div(std::move(e), num(d));
}

bool containsVar(const Ptr& e, const std::string& variable) {
	if (!e) return false;
	switch (e->op()) {
	case FnOp::Number: return false;
	case FnOp::Variable: return e->name() == variable;
	case FnOp::Negate:
	case FnOp::Call: return containsVar(e->lhs(), variable);
	default: return containsVar(e->lhs(), variable) || containsVar(e->rhs(), variable);
	}
}

std::string show(const Ptr& e) { return toString(simplify(e)); }

Ptr replaceSubexpression(const Ptr& e, const Ptr& target, const Ptr& replacement) {
	if (!e) return e;
	if (toString(e) == toString(target)) return replacement;
	switch (e->op()) {
	case FnOp::Number:
	case FnOp::Variable: return e;
	case FnOp::Negate: return FnExpr::unary(FnOp::Negate, replaceSubexpression(e->lhs(), target, replacement));
	case FnOp::Call: return FnExpr::call(e->func(), replaceSubexpression(e->lhs(), target, replacement));
	default:
		return FnExpr::binary(e->op(), replaceSubexpression(e->lhs(), target, replacement),
			replaceSubexpression(e->rhs(), target, replacement));
	}
}

void collectSubexpressions(const Ptr& e, std::vector<Ptr>& out) {
	if (!e) return;
	if (e->op() == FnOp::Number || e->op() == FnOp::Variable) return;
	const std::string key = toString(e);
	const bool seen = std::any_of(out.begin(), out.end(), [&](const Ptr& p) { return toString(p) == key; });
	if (!seen) out.push_back(e);
	collectSubexpressions(e->lhs(), out);
	collectSubexpressions(e->rhs(), out);
}

std::optional<Linear> asLinear(const Ptr& e, const std::string& variable) {
	if (!containsVar(e, variable)) return std::nullopt;
	const auto p = toPolynomial(e);
	if (!p || polynomialDegree(*p) != 1) return std::nullopt;
	return Linear{(*p)[1], (*p)[0]};
}

namespace {

void collectFactors(const Ptr& e, bool inverse, const std::string& variable, Factors& out) {
	if (!containsVar(e, variable)) {
		const double value = evaluate(e, 0.0);
		if (inverse) out.coefficient /= value;
		else out.coefficient *= value;
		return;
	}
	switch (e->op()) {
	case FnOp::Multiply:
		collectFactors(e->lhs(), inverse, variable, out);
		collectFactors(e->rhs(), inverse, variable, out);
		return;
	case FnOp::Divide:
		collectFactors(e->lhs(), inverse, variable, out);
		collectFactors(e->rhs(), !inverse, variable, out);
		return;
	case FnOp::Negate:
		out.coefficient = -out.coefficient;
		collectFactors(e->lhs(), inverse, variable, out);
		return;
	default:
		(inverse ? out.denominators : out.numerators).push_back(e);
	}
}

} // namespace

Factors flattenFactors(const Ptr& e, const std::string& variable) {
	Factors f;
	collectFactors(e, false, variable, f);
	return f;
}

Ptr rebuildProduct(const Factors& factors) {
	Ptr p;
	for (const Ptr& n : factors.numerators) p = p ? mul(p, n) : n;
	if (!p) p = num(1.0);
	for (const Ptr& d : factors.denominators) p = div(p, d);
	if (factors.coefficient != 1.0) p = mul(coefficient(factors.coefficient), p);
	return p;
}

Ptr normalizeProduct(const Ptr& e, const std::string& variable) {
	const Factors f = flattenFactors(e, variable);
	struct Term {
		Ptr base;
		double exponent;
		std::string key;
	};
	std::vector<Term> terms;
	auto addTerm = [&](const Ptr& factor, double sign) {
		Ptr base = factor;
		double exponent = 1.0;
		if (factor->op() == FnOp::Power && !containsVar(factor->rhs(), variable)) {
			base = factor->lhs();
			exponent = evaluate(factor->rhs(), 0.0);
		}
		const std::string key = toString(base);
		for (Term& t : terms) {
			if (t.key == key) {
				t.exponent += sign * exponent;
				return;
			}
		}
		terms.push_back({base, sign * exponent, key});
	};
	for (const Ptr& n : f.numerators) addTerm(n, 1.0);
	for (const Ptr& d : f.denominators) addTerm(d, -1.0);

	Factors out;
	out.coefficient = f.coefficient;
	for (const Term& t : terms) {
		if (std::fabs(t.exponent) < 1e-12) continue;
		const double magnitude = std::fabs(t.exponent);
		const Ptr factor = std::fabs(magnitude - 1.0) < 1e-12 ? t.base : pow(t.base, coefficient(magnitude));
		(t.exponent > 0.0 ? out.numerators : out.denominators).push_back(factor);
	}
	return rebuildProduct(out);
}

namespace {
thread_local int g_budget = 0;
}

void resetBudget(int calls) { g_budget = calls; }
bool consumeBudget() { return g_budget-- > 0; }

Ptr integratePolynomialExpr(const Coefficients& c, const std::string& variable) {
	Ptr result;
	for (int i = polynomialDegree(c); i >= 0; --i) {
		if (std::fabs(c[i]) < 1e-12) continue;
		const double r = c[i] / (i + 1);
		const bool negative = r < 0.0;
		const double magnitude = std::fabs(c[i]);
		const Ptr power = (i + 1 == 1) ? var(variable) : pow(var(variable), num(i + 1));
		Ptr term;
		if (std::fabs(magnitude - std::round(magnitude)) < 1e-9) {
			const long long p = static_cast<long long>(std::llround(magnitude));
			const long long q = i + 1;
			const long long g = std::gcd(p, q);
			const long long pp = p / g;
			const long long qq = q / g;
			term = pp == 1 ? power : mul(num(static_cast<double>(pp)), power);
			if (qq != 1) term = div(term, num(static_cast<double>(qq)));
		} else {
			term = mul(coefficient(std::fabs(r)), power);
		}
		if (!result) result = negative ? neg(term) : term;
		else result = negative ? sub(result, term) : add(result, term);
	}
	return result ? result : num(0.0);
}

Ptr differentiate(const Ptr& e) { return simplify(differential_calculus::Differentiation::derive(e)); }

} // namespace aksiomat::integral_calculus::detail
