#include <algorithm>
#include <cmath>

#include "aksiomat/integral_calculus/IntegrationInternal.hpp"

namespace aksiomat::integral_calculus::detail {
namespace {

using differential_calculus::toString;

constexpr const char* kSubstitutionName = "_u";

// Redoslijed prema pravilu LIATE: sto prije postaje "u" u parcijalnoj integraciji.
int partsPriority(const Ptr& factor) {
	if (factor->op() == FnOp::Call) {
		switch (factor->func()) {
		case FnFunc::Ln:
		case FnFunc::Asin:
		case FnFunc::Acos:
		case FnFunc::Atan: return 0;
		case FnFunc::Exp: return 3;
		default: return 2;
		}
	}
	if (factor->op() == FnOp::Power && factor->lhs()->op() == FnOp::Number) return 3;
	return 1;
}

Ptr productOf(const std::vector<Ptr>& items) {
	Ptr p;
	for (const Ptr& i : items) p = p ? mul(p, i) : i;
	return p ? p : num(1.0);
}

} // namespace

Outcome integrateBySubstitution(const Ptr& f, const std::string& v, int depth) {
	std::vector<Ptr> candidates;
	collectSubexpressions(f, candidates);
	const std::string t = kSubstitutionName;
	for (const Ptr& u : candidates) {
		if (!containsVar(u, v)) continue;
		if (u->op() == FnOp::Negate) continue;
		if (asLinear(u, v)) continue;
		const Ptr du = differentiate(u);
		if (!du || (du->op() == FnOp::Number && du->value() == 0.0)) continue;
		const Ptr g = normalizeProduct(div(f, du), v);
		const Ptr substituted = replaceSubexpression(g, u, var(t));
		if (containsVar(substituted, v)) continue;
		const auto inner = integrate(substituted, t, depth + 1);
		if (!inner) continue;
		Attempt a;
		a.antiderivative = replaceSubexpression(inner->antiderivative, var(t), u);
		a.method = "supstitucija";
		a.steps.push_back("Supstitucija: u = " + show(u) + ", pa je du = (" + show(du) + ") dx. Podintegralna funkcija "
			"se svede na g(u) = " + show(substituted) + ".");
		for (const auto& s : inner->steps) a.steps.push_back(s);
		a.steps.push_back("Vratimo supstituciju u = " + show(u) + ".");
		return a;
	}
	return std::nullopt;
}

Outcome integrateByParts(const Ptr& f, const std::string& v, int depth) {
	const Factors fl = flattenFactors(f, v);
	if (!fl.denominators.empty()) return std::nullopt;
	std::vector<Ptr> parts = fl.numerators;
	if (parts.size() < 2 && !(parts.size() == 1 && parts[0]->op() == FnOp::Call)) return std::nullopt;
	if (parts.size() > 4) return std::nullopt;

	std::vector<std::size_t> order(parts.size());
	for (std::size_t i = 0; i < order.size(); ++i) order[i] = i;
	std::stable_sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) {
		return partsPriority(parts[a]) < partsPriority(parts[b]);
	});

	for (const std::size_t idx : order) {
		const Ptr u = parts[idx];
		std::vector<Ptr> rest;
		for (std::size_t i = 0; i < parts.size(); ++i)
			if (i != idx) rest.push_back(parts[i]);
		Ptr dv = productOf(rest);
		if (fl.coefficient != 1.0) dv = mul(coefficient(fl.coefficient), dv);

		const auto vInt = integrate(dv, v, depth + 1);
		if (!vInt) continue;
		const Ptr du = differentiate(u);
		const Ptr next = normalizeProduct(mul(vInt->antiderivative, du), v);
		const auto tail = integrate(next, v, depth + 1);
		if (!tail) continue;

		Attempt a;
		a.antiderivative = sub(mul(u, vInt->antiderivative), tail->antiderivative);
		a.method = "parcijalna integracija";
		a.steps.push_back("Parcijalna integracija ∫ u dv = u·v - ∫ v du uz u = " + show(u) + ", dv = " + show(dv) +
			" dx. Tada je du = (" + show(du) + ") dx, v = " + show(vInt->antiderivative) + ".");
		for (const auto& s : vInt->steps) a.steps.push_back("(odredivanje v) " + s);
		a.steps.push_back("Preostaje integral ∫ v du = ∫ " + show(next) + " dx.");
		for (const auto& s : tail->steps) a.steps.push_back(s);
		return a;
	}
	return std::nullopt;
}

} // namespace aksiomat::integral_calculus::detail
