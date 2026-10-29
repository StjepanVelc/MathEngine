#include "aksiomat/integral_calculus/IndefiniteIntegral.hpp"

#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/integral_calculus/IntegrationInternal.hpp"

namespace aksiomat::integral_calculus {
namespace detail {

using differential_calculus::formatValue;

Outcome integrate(const Ptr& f, const std::string& v, int depth) {
	if (depth > kMaxDepth || !consumeBudget()) return std::nullopt;

	if (!containsVar(f, v)) {
		Attempt a;
		a.antiderivative = mul(f, var(v));
		a.method = "konstanta";
		a.steps.push_back("Integral konstante: ∫ c dx = c·x.");
		return a;
	}

	if (const auto poly = toPolynomial(f)) {
		Attempt a;
		a.antiderivative = integratePolynomialExpr(*poly, v);
		a.method = "polinom";
		a.steps.push_back("Polinom integriramo clan po clan (linearnost) pravilom potencije: ∫ x^n dx = x^(n+1)/(n+1), "
			"a konstantni faktori se izlucuju ispred integrala.");
		return a;
	}

	if (f->op() == FnOp::Negate) {
		if (auto r = integrate(f->lhs(), v, depth + 1)) {
			r->antiderivative = neg(r->antiderivative);
			r->steps.insert(r->steps.begin(), "Konstantni faktor -1 izlucimo: ∫ -g dx = -∫ g dx.");
			return r;
		}
		return std::nullopt;
	}

	if (f->op() == FnOp::Add || f->op() == FnOp::Subtract) {
		auto left = integrate(f->lhs(), v, depth + 1);
		auto right = left ? integrate(f->rhs(), v, depth + 1) : Outcome{};
		if (!left || !right) return std::nullopt;
		Attempt a;
		const bool plus = f->op() == FnOp::Add;
		a.antiderivative = plus ? add(left->antiderivative, right->antiderivative)
								: sub(left->antiderivative, right->antiderivative);
		a.method = left->method == right->method ? left->method : "linearnost";
		a.steps.push_back(std::string("Linearnost integrala: ∫ (g ") + (plus ? "+" : "-") + " h) dx = ∫ g dx " +
			(plus ? "+" : "-") + " ∫ h dx. Integriramo svaki clan zasebno.");
		a.steps.push_back("Prvi clan: " + show(f->lhs()));
		for (const auto& s : left->steps) a.steps.push_back(s);
		a.steps.push_back("Drugi clan: " + show(f->rhs()));
		for (const auto& s : right->steps) a.steps.push_back(s);
		return a;
	}

	const Factors fl = flattenFactors(f, v);
	if (std::fabs(fl.coefficient - 1.0) > 1e-12 && fl.coefficient != 0.0) {
		Factors rest = fl;
		rest.coefficient = 1.0;
		if (auto r = integrate(rebuildProduct(rest), v, depth + 1)) {
			r->antiderivative = scaleBy(fl.coefficient, r->antiderivative);
			r->steps.insert(r->steps.begin(), "Konstantu " + formatValue(fl.coefficient) +
				" izlucimo ispred integrala: ∫ c·g dx = c·∫ g dx.");
			return r;
		}
		return std::nullopt;
	}

	if (const auto t = tableIntegral(f, v, depth)) return t;
	if (const auto r = integrateRational(f, v, depth)) return r;
	if (const auto s = integrateBySubstitution(f, v, depth)) return s;
	if (const auto p = integrateByParts(f, v, depth)) return p;
	return std::nullopt;
}

} // namespace detail

IndefiniteIntegralResult IndefiniteIntegration::integrate(const std::string& expression, const std::string& variable) {
	using namespace detail;
	const Ptr f = differential_calculus::parseFunction(expression, variable);
	IndefiniteIntegralResult r;
	r.expression = differential_calculus::toString(differential_calculus::simplify(f));

	resetBudget(4000);
	const Outcome outcome = detail::integrate(f, variable, 0);
	if (!outcome) {
		r.message = "Antiderivacija nije pronadena podrzanim metodama (tablica, linearnost, supstitucija, "
			"parcijalna integracija, racionalne funkcije). Moguce je da se ne izrazava elementarnim funkcijama.";
		r.steps.push_back("Pokusane metode: tablicni integrali, linearnost, supstitucija, parcijalna integracija, rastav na parcijalne razlomke.");
		return r;
	}

	const Ptr F = differential_calculus::simplify(outcome->antiderivative);
	r.found = true;
	r.antiderivative = differential_calculus::toString(F);
	r.antiderivativeWithC = r.antiderivative + " + C";
	r.method = outcome->method;
	r.steps = outcome->steps;

	const Ptr dF = differential_calculus::simplify(differential_calculus::Differentiation::derive(F));
	r.verified = isAntiderivative(r.antiderivative, expression, variable);
	r.verification = "F'(" + variable + ") = " + differential_calculus::toString(dF) +
		(r.verified ? "  ->  podudara se s podintegralnom funkcijom, rezultat je provjeren."
					: "  ->  numericka provjera se ne podudara posvuda; rezultat treba uzeti s oprezom.");
	r.steps.push_back("Provjera: derivacija rezultata je " + r.verification);
	r.steps.push_back("Neodredeni integral je skup svih antiderivacija pa dodajemo konstantu C.");
	return r;
}

bool IndefiniteIntegration::isAntiderivative(const std::string& antiderivative, const std::string& integrand,
	const std::string& variable) {
	const auto F = differential_calculus::parseFunction(antiderivative, variable);
	const auto dF = differential_calculus::Differentiation::derive(F);
	return differential_calculus::equivalentFunctions(differential_calculus::toString(dF), integrand, variable);
}

} // namespace aksiomat::integral_calculus
