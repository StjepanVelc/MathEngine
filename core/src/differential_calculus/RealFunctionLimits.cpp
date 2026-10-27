#include "aksiomat/differential_calculus/RealFunctionLimits.hpp"

#include <cmath>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;

constexpr double kInf = std::numeric_limits<double>::infinity();
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

std::string fmt(double v) { return formatValue(v); }

double snapNice(double value) { return snapToNice(value); }

OneSidedLimit makeFinite(double v) {
	OneSidedLimit limit;
	limit.kind = LimitKind::Finite;
	limit.value = snapNice(v);
	return limit;
}

OneSidedLimit makeInfinite(double sign) {
	OneSidedLimit limit;
	limit.kind = sign > 0 ? LimitKind::PositiveInfinity : LimitKind::NegativeInfinity;
	limit.value = sign > 0 ? kInf : -kInf;
	return limit;
}

OneSidedLimit makeKind(LimitKind kind) {
	OneSidedLimit limit;
	limit.kind = kind;
	return limit;
}

OneSidedLimit classify(const std::vector<double>& v) {
	const std::size_t n = v.size();
	const double v3 = v[n - 3];
	const double v4 = v[n - 2];
	const double v5 = v[n - 1];
	if (std::isnan(v3) || std::isnan(v4) || std::isnan(v5)) return makeKind(LimitKind::Undefined);
	if (std::isinf(v5)) return makeInfinite(v5);

	const double d1 = v4 - v3;
	const double d2 = v5 - v4;
	const double scale = 1.0 + std::fabs(v5);
	double estimate = v5;
	bool converges = false;
	if (std::fabs(d2) <= 1e-3 * scale) {
		converges = true;
		if (std::fabs(d1) > 1e-9 * scale) {
			const double r = d2 / d1;
			if (r >= 0.0 && r < 0.95) estimate += d2 * r / (1.0 - r);
		}
	} else if (d1 != 0.0) {
		const double r = d2 / d1;
		if (r >= 0.0 && r < 0.95) {
			converges = true;
			estimate += d2 * r / (1.0 - r);
		}
	}
	if (converges) return makeFinite(estimate);

	if (n >= 4 && std::fabs(v5) > 10.0) {
		bool growing = true;
		for (std::size_t i = n - 3; i < n; ++i) {
			if (!(std::fabs(v[i]) > std::fabs(v[i - 1])) || std::isinf(v[i]) ||
				((v[i] > 0) != (v[i - 1] > 0))) {
				growing = false;
				break;
			}
		}
		if (growing) return makeInfinite(v5);
	}
	return makeKind(LimitKind::DoesNotExist);
}

OneSidedLimit numericLimit(const Ptr& f, double a, int direction) {
	std::vector<double> values;
	if (std::isinf(a)) {
		const double sign = a > 0 ? 1.0 : -1.0;
		for (int k = 3; k <= 7; ++k) values.push_back(evaluate(f, sign * std::pow(10.0, k)));
	} else {
		for (int k = 2; k <= 5; ++k) values.push_back(evaluate(f, a + direction * std::pow(10.0, -k)));
	}
	return classify(values);
}

// Limes s one strane koja je definirana (prvo zdesna, pa slijeva).
OneSidedLimit probe(const Ptr& f, double a) {
	OneSidedLimit result = numericLimit(f, a, 1);
	if (result.kind == LimitKind::Undefined && !std::isinf(a)) result = numericLimit(f, a, -1);
	return result;
}

bool isZero(const OneSidedLimit& l) { return l.kind == LimitKind::Finite && std::fabs(l.value) < 1e-9; }
bool isOne(const OneSidedLimit& l) { return l.kind == LimitKind::Finite && std::fabs(l.value - 1.0) < 1e-9; }
bool isInfinite(const OneSidedLimit& l) {
	return l.kind == LimitKind::PositiveInfinity || l.kind == LimitKind::NegativeInfinity;
}

OneSidedLimit combine(const OneSidedLimit& l, const OneSidedLimit& r) {
	if (l.kind == LimitKind::Undefined) return r;
	if (r.kind == LimitKind::Undefined) return l;
	if (l.kind == LimitKind::Finite && r.kind == LimitKind::Finite) {
		if (std::fabs(l.value - r.value) <= 1e-4 * (1.0 + std::fabs(l.value))) return makeFinite((l.value + r.value) / 2.0);
		return makeKind(LimitKind::DoesNotExist);
	}
	if (l.kind == r.kind && isInfinite(l)) return l;
	return makeKind(LimitKind::DoesNotExist);
}

Ptr unwrapNegation(const Ptr& f) {
	return f->op() == FnOp::Negate ? f->lhs() : f;
}

std::string detectForm(const Ptr& top, double a) {
	const Ptr f = unwrapNegation(top);
	if (f->op() != FnOp::Divide && f->op() != FnOp::Multiply && f->op() != FnOp::Add &&
		f->op() != FnOp::Subtract && f->op() != FnOp::Power) return {};
	const OneSidedLimit l = probe(f->lhs(), a);
	const OneSidedLimit r = probe(f->rhs(), a);
	switch (f->op()) {
	case FnOp::Divide:
		if (isZero(l) && isZero(r)) return "0/0";
		if (isInfinite(l) && isInfinite(r)) return "inf/inf";
		break;
	case FnOp::Multiply:
		if ((isZero(l) && isInfinite(r)) || (isInfinite(l) && isZero(r))) return "0*inf";
		break;
	case FnOp::Add:
		if (isInfinite(l) && isInfinite(r) && l.kind != r.kind) return "inf-inf";
		break;
	case FnOp::Subtract:
		if (isInfinite(l) && isInfinite(r) && l.kind == r.kind) return "inf-inf";
		break;
	case FnOp::Power:
		if (isOne(l) && isInfinite(r)) return "1^inf";
		if (isZero(l) && isZero(r)) return "0^0";
		if (isInfinite(l) && isZero(r)) return "inf^0";
		break;
	default: break;
	}
	return {};
}

struct HopitalOutcome {
	bool resolved = false;
	bool exact = false;
	OneSidedLimit limit;
};

HopitalOutcome lHopital(Ptr numerator, Ptr denominator, double a, std::vector<std::string>& steps) {
	HopitalOutcome outcome;
	for (int iteration = 1; iteration <= 4; ++iteration) {
		const Ptr dn = simplify(Differentiation::derive(numerator));
		const Ptr dd = simplify(Differentiation::derive(denominator));
		steps.push_back("L'Hopitalovo pravilo (" + std::to_string(iteration) +
			"): lim f/g = lim f'/g'. Deriviramo brojnik i nazivnik zasebno: f' = " + toString(dn) +
			",  g' = " + toString(dd) + ".");
		const Ptr quotient = simplify(FnExpr::binary(FnOp::Divide, dn, dd));
		if (!std::isinf(a)) {
			const double direct = evaluate(quotient, a);
			if (std::isfinite(direct)) {
				steps.push_back("Uvrstavanjem x = " + fmt(a) + " u f'/g' dobivamo " + fmt(direct) +
					", sto vise nije neodredjen oblik.");
				outcome.resolved = true;
				outcome.exact = true;
				outcome.limit = makeFinite(direct);
				return outcome;
			}
		}
		const OneSidedLimit ln = probe(dn, a);
		const OneSidedLimit ld = probe(dd, a);
		const bool again = (isZero(ln) && isZero(ld)) || (isInfinite(ln) && isInfinite(ld));
		if (!again) {
			outcome.resolved = true;
			outcome.limit = probe(quotient, a);
			steps.push_back("f'/g' = " + toString(quotient) + " vise nije neodredjen oblik; njegov limes je " +
				RealFunctionLimits::describe(outcome.limit) + ".");
			return outcome;
		}
		steps.push_back("Opet je oblik 0/0 (ili inf/inf), pa pravilo primjenjujemo ponovno.");
		numerator = dn;
		denominator = dd;
	}
	outcome.limit = probe(FnExpr::binary(FnOp::Divide, numerator, denominator), a);
	outcome.resolved = true;
	return outcome;
}

std::vector<LimitSample> buildSamples(const Ptr& f, double a, int direction) {
	std::vector<LimitSample> samples;
	if (std::isinf(a)) {
		const double sign = a > 0 ? 1.0 : -1.0;
		for (const double magnitude : {10.0, 50.0, 100.0, 1e3, 1e4, 1e5, 1e6}) {
			const double x = sign * magnitude;
			const double y = evaluate(f, x);
			samples.push_back({x, y, !std::isnan(y)});
		}
		return samples;
	}
	for (const double h : {0.5, 0.2, 0.1, 0.05, 0.01, 0.001, 0.0001}) {
		const double x = a + direction * h;
		const double y = evaluate(f, x);
		samples.push_back({x, y, !std::isnan(y)});
	}
	return samples;
}

std::string sampleText(const std::vector<LimitSample>& samples, std::size_t from) {
	std::string text;
	for (std::size_t i = from; i < samples.size(); ++i) {
		if (!text.empty()) text += ", ";
		text += samples[i].defined ? fmt(samples[i].value) : "nedefinirano";
	}
	return text;
}

} // namespace

std::string RealFunctionLimits::describe(const OneSidedLimit& limit) {
	switch (limit.kind) {
	case LimitKind::Finite: return fmt(limit.value);
	case LimitKind::PositiveInfinity: return "+inf";
	case LimitKind::NegativeInfinity: return "-inf";
	case LimitKind::DoesNotExist: return "ne postoji";
	case LimitKind::Undefined: return "nije definirano";
	}
	return {};
}

OneSidedLimit RealFunctionLimits::oneSided(const FnExpr::Ptr& expression, double point, int direction) {
	if (!expression) throw std::invalid_argument("Prazan izraz");
	if (std::isnan(point)) throw std::invalid_argument("Tocka limesa nije valjan broj");
	return numericLimit(expression, point, direction < 0 ? -1 : 1);
}

RealLimitResult RealFunctionLimits::evaluate(const std::string& expression, double point,
	const std::string& variable) {
	if (std::isnan(point)) throw std::invalid_argument("Tocka limesa nije valjan broj");
	const Ptr f = simplify(parseFunction(expression, variable));

	RealLimitResult result;
	result.expression = toString(f);
	result.point = point;
	result.atInfinity = std::isinf(point);
	const std::string pointText = fmt(point);
	result.steps.push_back("Trazimo limes funkcije f(" + variable + ") = " + result.expression + " kad " +
		variable + " -> " + pointText + ".");

	if (!result.atInfinity) {
		result.valueAtPoint = ::aksiomat::differential_calculus::evaluate(f, point);
		result.valueDefinedAtPoint = std::isfinite(result.valueAtPoint);
		if (result.valueDefinedAtPoint) {
			result.steps.push_back("Direktna supstitucija: f(" + pointText + ") = " + fmt(result.valueAtPoint) +
				". Limes ne ovisi o vrijednosti u samoj tocki, nego o vrijednostima oko nje - zato promatramo obje strane.");
		} else {
			result.steps.push_back("Direktna supstitucija ne daje konacan broj (f u toj tocki nije definirana ili je oblik poput 0/0). "
				"Zato promatramo vrijednosti f(x) za x sve blize tocki " + pointText + ".");
		}
		result.left = numericLimit(f, point, -1);
		result.right = numericLimit(f, point, 1);
		result.leftSamples = buildSamples(f, point, -1);
		result.rightSamples = buildSamples(f, point, 1);
		result.steps.push_back("Slijeva (x = " + pointText + " - 0.01, -0.001, -0.0001): f(x) = " +
			sampleText(result.leftSamples, 4) + "  =>  lijevi limes: " + describe(result.left) + ".");
		result.steps.push_back("Zdesna (x = " + pointText + " + 0.01, +0.001, +0.0001): f(x) = " +
			sampleText(result.rightSamples, 4) + "  =>  desni limes: " + describe(result.right) + ".");
	} else {
		result.left = numericLimit(f, point, point > 0 ? 1 : -1);
		result.right = result.left;
		result.leftSamples = buildSamples(f, point, 1);
		result.rightSamples = result.leftSamples;
		result.steps.push_back("Za sve vece |x| (x = " + fmt(result.leftSamples[3].x) + ", " +
			fmt(result.leftSamples[4].x) + ", " + fmt(result.leftSamples[5].x) + ", " +
			fmt(result.leftSamples[6].x) + ") vrijednosti f(x) su: " + sampleText(result.leftSamples, 3) +
			"  =>  limes: " + describe(result.left) + ".");
	}

	result.combined = combine(result.left, result.right);

	result.indeterminateForm = detectForm(f, point);
	const Ptr core = unwrapNegation(f);
	if (!result.indeterminateForm.empty()) {
		result.steps.push_back("Prepoznat je neodredjen oblik: " + result.indeterminateForm +
			". Samo uvrstavanje ili 'pravilo limesa umnoska/kvocijenta' ovdje ne daje odgovor.");
		std::optional<OneSidedLimit> exact;
		const bool negated = f->op() == FnOp::Negate;
		const double sign = negated ? -1.0 : 1.0;

		auto adopt = [&](const OneSidedLimit& candidate) {
			if (candidate.kind == LimitKind::Finite) {
				exact = makeFinite(sign * candidate.value);
			} else if (isInfinite(candidate)) {
				exact = makeInfinite(sign * (candidate.kind == LimitKind::PositiveInfinity ? 1.0 : -1.0));
			}
		};

		if (result.indeterminateForm == "0/0" || result.indeterminateForm == "inf/inf") {
			result.steps.push_back("Za oblik 0/0 i inf/inf vrijedi L'Hopitalovo pravilo: ako limes kvocijenta derivacija postoji, jednak je trazenom limesu.");
			const HopitalOutcome outcome = lHopital(core->lhs(), core->rhs(), point, result.steps);
			result.lHopitalUsed = true;
			if (outcome.resolved) adopt(outcome.limit);
		} else if (result.indeterminateForm == "0*inf") {
			result.steps.push_back("Umnozak 0*inf pretvaramo u kvocijent: f*g = f/(1/g), cime dobivamo oblik 0/0 ili inf/inf.");
			const OneSidedLimit l = probe(core->lhs(), point);
			Ptr zeroPart = isZero(l) ? core->lhs() : core->rhs();
			Ptr infinitePart = isZero(l) ? core->rhs() : core->lhs();
			const Ptr reciprocal = simplify(FnExpr::binary(FnOp::Divide, FnExpr::number(1.0), infinitePart));
			const HopitalOutcome outcome = lHopital(zeroPart, reciprocal, point, result.steps);
			result.lHopitalUsed = true;
			if (outcome.resolved) adopt(outcome.limit);
		} else if (result.indeterminateForm == "inf-inf") {
			result.steps.push_back("Razliku beskonacnosti treba algebarski preurediti (zajednicki nazivnik, mnozenje konjugatom ili izlucivanje najveceg clana) prije racunanja limesa. Rezultat je odredjen numericki.");
		} else {
			result.steps.push_back("Potenciju u^v pisemo kao e^(v*ln u), pa je limes = e^(lim v*ln u).");
			const Ptr product = simplify(FnExpr::binary(FnOp::Multiply, core->rhs(), FnExpr::call(FnFunc::Ln, core->lhs())));
			const OneSidedLimit inner = probe(product, point);
			double value = kNaN;
			if (inner.kind == LimitKind::Finite) value = std::exp(inner.value);
			else if (inner.kind == LimitKind::NegativeInfinity) value = 0.0;
			else if (inner.kind == LimitKind::PositiveInfinity) value = kInf;
			result.steps.push_back("lim v*ln u = lim " + toString(product) + " = " + describe(inner) +
				"  =>  limes = e^(" + describe(inner) + ") = " + fmt(value) + ".");
			if (std::isinf(value)) adopt(makeInfinite(value));
			else if (!std::isnan(value)) adopt(makeFinite(value));
		}

		if (exact) {
			bool consistent = result.combined.kind == exact->kind;
			if (consistent && exact->kind == LimitKind::Finite) {
				consistent = std::fabs(result.combined.value - exact->value) <= 1e-3 * (1.0 + std::fabs(exact->value));
			}
			if (consistent) result.left = result.right = result.combined = *exact;
		}
	}

	std::string conclusion;
	switch (result.combined.kind) {
	case LimitKind::Finite:
		conclusion = "Lijevi i desni limes se podudaraju, pa limes postoji i iznosi " + describe(result.combined) + ".";
		if (!result.atInfinity && result.valueDefinedAtPoint &&
			std::fabs(result.valueAtPoint - result.combined.value) > 1e-6 * (1.0 + std::fabs(result.combined.value))) {
			conclusion += " Primijeti: limes je razlicit od f(" + pointText + ") = " + fmt(result.valueAtPoint) +
				", pa funkcija u toj tocki nije neprekidna.";
		}
		break;
	case LimitKind::PositiveInfinity:
		conclusion = "Vrijednosti neogranicano rastu: limes je +inf (nema konacnog limesa)."; break;
	case LimitKind::NegativeInfinity:
		conclusion = "Vrijednosti neogranicano padaju: limes je -inf (nema konacnog limesa)."; break;
	case LimitKind::DoesNotExist:
		conclusion = "Lijevi limes ("
			") se razlikuju ili vrijednosti osciliraju, pa limes ne postoji."; break;
	case LimitKind::Undefined:
		conclusion = "Funkcija nije definirana u okolini tocke, pa limes nema smisla."; break;
	}
	result.steps.push_back(conclusion);
	return result;
}

} // namespace aksiomat::differential_calculus
