#include "aksiomat/integral_calculus/IntegralCalculusJson.hpp"

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/integral_calculus/DefiniteIntegral.hpp"
#include "aksiomat/integral_calculus/IndefiniteIntegral.hpp"
#include "aksiomat/integral_calculus/OrdinaryDifferentialEquations.hpp"
#include "aksiomat/integral_calculus/RiemannSums.hpp"
#include "aksiomat/integral_calculus/SequenceLimits.hpp"

namespace aksiomat::integral_calculus {
namespace {

std::string num(double v) {
	if (!std::isfinite(v)) return "null";
	char buffer[64];
	std::snprintf(buffer, sizeof buffer, "%.10g", v);
	return buffer;
}

std::string str(const std::string& value) {
	std::string out = "\"";
	for (const char c : value) {
		switch (c) {
		case '"': out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) out += ' ';
			else out += c;
		}
	}
	return out + "\"";
}

std::string boolean(bool b) { return b ? "true" : "false"; }

std::string strings(const std::vector<std::string>& values) {
	std::string out = "[";
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (i) out += ",";
		out += str(values[i]);
	}
	return out + "]";
}

std::string numbers(const std::vector<double>& values) {
	std::string out = "[";
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (i) out += ",";
		out += num(values[i]);
	}
	return out + "]";
}

std::string kindId(ImproperKind k) {
	switch (k) {
	case ImproperKind::Convergent: return "convergent";
	case ImproperKind::Divergent: return "divergent";
	case ImproperKind::Unknown: return "unknown";
	}
	return "unknown";
}

std::string improperJson(const ImproperIntegralInfo& r) {
	return "{\"expression\":" + str(r.expression) + ",\"lower\":" + num(r.lower) + ",\"upper\":" + num(r.upper) +
		",\"lowerInfinite\":" + boolean(r.lowerInfinite) + ",\"upperInfinite\":" + boolean(r.upperInfinite) +
		",\"kind\":" + str(kindId(r.kind)) + ",\"value\":" + (r.kind == ImproperKind::Convergent ? num(r.value) : "null") +
		",\"bounds\":" + numbers(r.bounds) + ",\"partials\":" + numbers(r.partials) + ",\"steps\":" + strings(r.steps) + "}";
}

std::string definiteJson(const DefiniteIntegralResult& r) {
	return "{\"expression\":" + str(r.expression) + ",\"lower\":" + num(r.lower) + ",\"upper\":" + num(r.upper) +
		",\"symbolic\":" + boolean(r.symbolic) + ",\"antiderivative\":" + str(r.antiderivative) + ",\"value\":" + num(r.value) +
		",\"numericCheck\":" + num(r.numericCheck) + ",\"agrees\":" + boolean(r.agrees) + ",\"method\":" + str(r.method) +
		",\"steps\":" + strings(r.steps) + "}";
}

std::string odeJson(const OdeSolution& r) {
	return "{\"type\":" + str(r.type) + ",\"equation\":" + str(r.equation) + ",\"general\":" + str(r.general) +
		",\"particular\":" + str(r.particular) + ",\"explicit\":" + boolean(r.explicitForm) + ",\"hasInitial\":" + boolean(r.hasInitial) +
		",\"x0\":" + num(r.x0) + ",\"y0\":" + num(r.y0) + ",\"constant\":" + num(r.constant) +
		",\"xs\":" + numbers(r.xs) + ",\"ys\":" + numbers(r.ys) + ",\"verified\":" + boolean(r.verified) +
		",\"residual\":" + num(r.residual) + ",\"verification\":" + str(r.verification) + ",\"steps\":" + strings(r.steps) + "}";
}

std::string inductionJson(const InductionResult& r) {
	return "{\"statement\":" + str(r.statement) + ",\"baseCaseHolds\":" + boolean(r.baseCaseHolds) +
		",\"stepVerified\":" + boolean(r.inductionStepVerified) + ",\"checked\":" + std::to_string(r.checked) +
		",\"firstCounterexample\":" + std::to_string(r.firstCounterexample) + ",\"steps\":" + strings(r.steps) + "}";
}

} // namespace

std::string IntegralCalculusJson::riemann(const std::string& expression, double a, double b, int n, const std::string& method) {
	const auto r = RiemannSums::compute(expression, a, b, n, RiemannSums::parseMethod(method));
	std::string rects = "[";
	for (std::size_t i = 0; i < r.rectangles.size(); ++i) {
		if (i) rects += ",";
		const auto& q = r.rectangles[i];
		rects += "{\"x0\":" + num(q.x0) + ",\"x1\":" + num(q.x1) + ",\"height\":" + num(q.height) + ",\"area\":" + num(q.area) + "}";
	}
	rects += "]";
	return "{\"expression\":" + str(r.expression) + ",\"method\":" + str(r.method) + ",\"a\":" + num(r.a) +
		",\"b\":" + num(r.b) + ",\"n\":" + std::to_string(r.n) + ",\"approximation\":" + num(r.approximation) +
		",\"stepWidth\":" + num(r.stepWidth) + ",\"lowerSum\":" + num(r.lowerSum) + ",\"upperSum\":" + num(r.upperSum) +
		",\"reference\":" + num(r.reference) + ",\"error\":" + num(r.error) + ",\"rectangles\":" + rects +
		",\"steps\":" + strings(r.steps) + "}";
}

std::string IntegralCalculusJson::indefinite(const std::string& expression) {
	const auto r = IndefiniteIntegration::integrate(expression);
	return "{\"expression\":" + str(r.expression) + ",\"found\":" + boolean(r.found) + ",\"antiderivative\":" + str(r.antiderivative) +
		",\"withC\":" + str(r.antiderivativeWithC) + ",\"method\":" + str(r.method) + ",\"verified\":" + boolean(r.verified) +
		",\"verification\":" + str(r.verification) + ",\"message\":" + str(r.message) + ",\"steps\":" + strings(r.steps) + "}";
}

std::string IntegralCalculusJson::definite(const std::string& expression, double a, double b) {
	return definiteJson(DefiniteIntegration::integrate(expression, a, b));
}

std::string IntegralCalculusJson::areaBetween(const std::string& f, const std::string& g, double a, double b) {
	const auto r = DefiniteIntegration::areaBetween(f, g, a, b);
	return "{\"upperCurve\":" + str(r.upperCurve) + ",\"lowerCurve\":" + str(r.lowerCurve) + ",\"lower\":" + num(r.lower) +
		",\"upper\":" + num(r.upper) + ",\"area\":" + num(r.area) + ",\"crossings\":" + numbers(r.crossings) +
		",\"steps\":" + strings(r.steps) + "}";
}

std::string IntegralCalculusJson::improper(const std::string& expression, const std::string& kind, double a, double b) {
	if (kind == "upper") return improperJson(DefiniteIntegration::improper(expression, a, false, 0.0, true));
	if (kind == "lower") return improperJson(DefiniteIntegration::improper(expression, 0.0, true, b, false));
	if (kind == "both") return improperJson(DefiniteIntegration::improper(expression, 0.0, true, 0.0, true));
	if (kind == "singular_lower") return improperJson(DefiniteIntegration::improperSingular(expression, a, b, true));
	if (kind == "singular_upper") return improperJson(DefiniteIntegration::improperSingular(expression, a, b, false));
	if (kind == "ptest_infinity") return improperJson(DefiniteIntegration::pTest(a, true));
	if (kind == "ptest_zero") return improperJson(DefiniteIntegration::pTest(a, false));
	throw std::invalid_argument("Nepoznata vrsta nepravog integrala: " + kind);
}

std::string IntegralCalculusJson::meanValue(const std::string& expression, double a, double b) {
	return definiteJson(DefiniteIntegration::meanValue(expression, a, b));
}

std::string IntegralCalculusJson::ode(const std::string& kind, const std::string& p1, const std::string& p2, bool hasInitial,
	double x0, double y0, double xEnd, double extra) {
	if (kind == "separable") return odeJson(OrdinaryDifferentialEquations::separable(p1, p2, hasInitial, x0, y0, xEnd));
	if (kind == "linear") return odeJson(OrdinaryDifferentialEquations::linearFirstOrder(p1, p2, hasInitial, x0, y0, xEnd));
	if (kind == "growth") return odeJson(OrdinaryDifferentialEquations::exponentialGrowth(x0, y0, xEnd));
	if (kind == "logistic") return odeJson(OrdinaryDifferentialEquations::logistic(x0, extra, y0, xEnd));
	throw std::invalid_argument("Nepoznata vrsta diferencijalne jednadzbe: " + kind);
}

std::string IntegralCalculusJson::induction(const std::string& kind, const std::string& left, const std::string& right, int n0, int maxN) {
	if (kind == "sum") return inductionJson(SequenceLimits::verifySumIdentity(left, right, maxN));
	if (kind == "inequality") return inductionJson(SequenceLimits::verifyInequality(left, right, n0, maxN));
	throw std::invalid_argument("Nepoznata vrsta indukcije: " + kind);
}

std::string IntegralCalculusJson::sequence(const std::string& expression, double epsilon, int termCount) {
	const auto r = SequenceLimits::analyze(expression, epsilon, termCount);
	return "{\"expression\":" + str(r.expression) + ",\"converges\":" + boolean(r.converges) + ",\"divergesToInfinity\":" +
		boolean(r.divergesToInfinity) + ",\"limit\":" + (r.converges ? num(r.limit) : "null") + ",\"monotone\":" + boolean(r.monotone) +
		",\"monotonicity\":" + str(r.monotonicity) + ",\"bounded\":" + boolean(r.bounded) + ",\"lowerBound\":" + num(r.lowerBound) +
		",\"upperBound\":" + num(r.upperBound) + ",\"epsilon\":" + num(r.epsilon) + ",\"epsilonN\":" + std::to_string(r.epsilonN) +
		",\"terms\":" + numbers(r.terms) + ",\"steps\":" + strings(r.steps) + "}";
}

std::string IntegralCalculusJson::sample(const std::string& expression, double xMin, double xMax, int count) {
	if (!std::isfinite(xMin) || !std::isfinite(xMax) || !(xMin < xMax)) throw std::invalid_argument("Potrebno je konacno xMin < xMax.");
	if (count < 2 || count > 5000) throw std::invalid_argument("Broj tocaka mora biti izmedu 2 i 5000.");
	const auto f = differential_calculus::parseFunction(expression);
	std::string out = "[";
	for (int i = 0; i < count; ++i) {
		const double x = xMin + (xMax - xMin) * i / (count - 1);
		if (i) out += ",";
		out += "{\"x\":" + num(x) + ",\"y\":" + num(differential_calculus::evaluate(f, x)) + "}";
	}
	return out + "]";
}

} // namespace aksiomat::integral_calculus
