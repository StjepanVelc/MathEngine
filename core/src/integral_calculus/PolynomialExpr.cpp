#include "aksiomat/integral_calculus/PolynomialExpr.hpp"

#include <cmath>

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::FnExpr;
using differential_calculus::FnOp;
using differential_calculus::dependsOnVariable;
using differential_calculus::evaluate;
using Ptr = FnExpr::Ptr;

constexpr double kEps = 1e-12;
constexpr std::size_t kMaxSize = 13;

void trim(Coefficients& c) {
	while (c.size() > 1 && std::fabs(c.back()) < kEps) c.pop_back();
	if (c.empty()) c.push_back(0.0);
}

Coefficients multiply(const Coefficients& a, const Coefficients& b) {
	Coefficients r(a.size() + b.size() - 1, 0.0);
	for (std::size_t i = 0; i < a.size(); ++i)
		for (std::size_t j = 0; j < b.size(); ++j) r[i + j] += a[i] * b[j];
	trim(r);
	return r;
}

} // namespace

std::optional<Coefficients> toPolynomial(const Ptr& e) {
	if (!e) return std::nullopt;
	if (!dependsOnVariable(e)) {
		const double v = evaluate(e, 0.0);
		if (!std::isfinite(v)) return std::nullopt;
		return Coefficients{v};
	}
	switch (e->op()) {
	case FnOp::Variable: return Coefficients{0.0, 1.0};
	case FnOp::Negate: {
		auto a = toPolynomial(e->lhs());
		if (!a) return std::nullopt;
		for (double& v : *a) v = -v;
		return a;
	}
	case FnOp::Add:
	case FnOp::Subtract: {
		auto a = toPolynomial(e->lhs());
		auto b = toPolynomial(e->rhs());
		if (!a || !b) return std::nullopt;
		Coefficients r(std::max(a->size(), b->size()), 0.0);
		for (std::size_t i = 0; i < a->size(); ++i) r[i] += (*a)[i];
		for (std::size_t i = 0; i < b->size(); ++i) r[i] += (e->op() == FnOp::Add ? 1.0 : -1.0) * (*b)[i];
		trim(r);
		return r;
	}
	case FnOp::Multiply: {
		auto a = toPolynomial(e->lhs());
		auto b = toPolynomial(e->rhs());
		if (!a || !b) return std::nullopt;
		auto r = multiply(*a, *b);
		if (r.size() > kMaxSize) return std::nullopt;
		return r;
	}
	case FnOp::Divide: {
		auto a = toPolynomial(e->lhs());
		auto b = toPolynomial(e->rhs());
		if (!a || !b || b->size() != 1 || std::fabs((*b)[0]) < kEps) return std::nullopt;
		for (double& v : *a) v /= (*b)[0];
		return a;
	}
	case FnOp::Power: {
		if (dependsOnVariable(e->rhs())) return std::nullopt;
		const double n = evaluate(e->rhs(), 0.0);
		if (!std::isfinite(n) || n < 0.0 || std::fabs(n - std::round(n)) > kEps || n > 12.0) return std::nullopt;
		auto base = toPolynomial(e->lhs());
		if (!base) return std::nullopt;
		Coefficients r{1.0};
		for (int i = 0; i < static_cast<int>(std::round(n)); ++i) {
			r = multiply(r, *base);
			if (r.size() > kMaxSize) return std::nullopt;
		}
		return r;
	}
	default: return std::nullopt;
	}
}

int polynomialDegree(const Coefficients& c) {
	int d = static_cast<int>(c.size()) - 1;
	while (d > 0 && std::fabs(c[d]) < kEps) --d;
	return d < 0 ? 0 : d;
}

Ptr fromPolynomial(const Coefficients& c, const std::string& variable) {
	Ptr result;
	for (int i = polynomialDegree(c); i >= 0; --i) {
		const double v = c[i];
		if (std::fabs(v) < kEps) continue;
		const double magnitude = std::fabs(v);
		Ptr power;
		if (i == 0) power = nullptr;
		else if (i == 1) power = FnExpr::variable(variable);
		else power = FnExpr::binary(FnOp::Power, FnExpr::variable(variable), FnExpr::number(i));

		Ptr term;
		if (!power) term = FnExpr::number(magnitude);
		else if (std::fabs(magnitude - 1.0) < kEps) term = power;
		else term = FnExpr::binary(FnOp::Multiply, FnExpr::number(magnitude), power);

		if (!result) result = v < 0.0 ? FnExpr::unary(FnOp::Negate, term) : term;
		else result = FnExpr::binary(v < 0.0 ? FnOp::Subtract : FnOp::Add, result, term);
	}
	return result ? result : FnExpr::number(0.0);
}

} // namespace aksiomat::integral_calculus
