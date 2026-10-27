#include "aksiomat/differential_calculus/DifferentialCalculusJson.hpp"

#include <cmath>
#include <cstdio>
#include <limits>
#include <stdexcept>


#include "aksiomat/differential_calculus/DerivativeAtPoint.hpp"
#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/FunctionContinuity.hpp"
#include "aksiomat/differential_calculus/FunctionStudy.hpp"
#include "aksiomat/differential_calculus/RealFunctionLimits.hpp"

namespace aksiomat::differential_calculus {
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

std::string steps(const std::vector<std::string>& values) {
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

std::string limitKind(LimitKind kind) {
	switch (kind) {
	case LimitKind::Finite: return "finite";
	case LimitKind::PositiveInfinity: return "plus_infinity";
	case LimitKind::NegativeInfinity: return "minus_infinity";
	case LimitKind::DoesNotExist: return "does_not_exist";
	case LimitKind::Undefined: return "undefined";
	}
	return "undefined";
}

std::string limitJson(const OneSidedLimit& l) {
	return "{\"kind\":" + str(limitKind(l.kind)) + ",\"value\":" + num(l.value) +
		",\"text\":" + str(RealFunctionLimits::describe(l)) + "}";
}

std::string samplesJson(const std::vector<LimitSample>& samples) {
	std::string out = "[";
	for (std::size_t i = 0; i < samples.size(); ++i) {
		if (i) out += ",";
		out += "{\"x\":" + num(samples[i].x) + ",\"y\":" + (samples[i].defined ? num(samples[i].value) : "null") + "}";
	}
	return out + "]";
}

std::string pointsJson(const std::vector<FunctionPoint>& points) {
	std::string out = "[";
	for (std::size_t i = 0; i < points.size(); ++i) {
		if (i) out += ",";
		out += "[" + num(points[i].x) + "," + (points[i].defined ? num(points[i].y) : "null") + "]";
	}
	return out + "]";
}

std::string intervalsJson(const std::vector<IntervalBehavior>& list) {
	std::string out = "[";
	for (std::size_t i = 0; i < list.size(); ++i) {
		if (i) out += ",";
		out += "{\"from\":" + num(list[i].from) + ",\"to\":" + num(list[i].to) + ",\"behavior\":" + str(list[i].behavior) + "}";
	}
	return out + "]";
}

double parsePoint(const std::string& text) {
	std::string t;
	for (const char c : text) if (c != ' ') t += c;
	if (t == "inf" || t == "+inf" || t == "infty") return std::numeric_limits<double>::infinity();
	if (t == "-inf" || t == "-infty") return -std::numeric_limits<double>::infinity();
	if (t.empty()) throw std::invalid_argument("Tocka limesa je prazna");
	try {
		std::size_t used = 0;
		const double v = std::stod(t, &used);
		if (used != t.size() || !std::isfinite(v)) throw std::invalid_argument("x");
		return v;
	} catch (const std::exception&) {
		throw std::invalid_argument("Tocka limesa nije valjan broj (dopusteno je i inf, -inf)");
	}
}

} // namespace

std::string DifferentialCalculusJson::limit(const std::string& expression, const std::string& pointText) {
	const double point = parsePoint(pointText);
	const auto r = RealFunctionLimits::evaluate(expression, point);
	return "{\"expression\":" + str(r.expression) + ",\"point\":" + num(r.point) +
		",\"atInfinity\":" + boolean(r.atInfinity) + ",\"pointSign\":" + (r.atInfinity ? (point > 0 ? "1" : "-1") : "0") +
		",\"valueDefined\":" + boolean(r.valueDefinedAtPoint) + ",\"valueAtPoint\":" + num(r.valueAtPoint) +
		",\"left\":" + limitJson(r.left) + ",\"right\":" + limitJson(r.right) + ",\"combined\":" + limitJson(r.combined) +
		",\"indeterminateForm\":" + str(r.indeterminateForm) + ",\"lHopital\":" + boolean(r.lHopitalUsed) +
		",\"leftSamples\":" + samplesJson(r.leftSamples) + ",\"rightSamples\":" + samplesJson(r.rightSamples) +
		",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::derivative(const std::string& expression, int order) {
	const auto r = Differentiation::differentiate(expression, order);
	return "{\"expression\":" + str(r.expression) + ",\"order\":" + std::to_string(r.order) +
		",\"raw\":" + str(r.rawDerivative) + ",\"derivative\":" + str(r.derivative) +
		",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::continuity(const std::string& expression, double point) {
	const auto r = FunctionContinuity::analyze(expression, point);
	return "{\"expression\":" + str(r.expression) + ",\"point\":" + num(r.point) +
		",\"valueDefined\":" + boolean(r.valueDefined) + ",\"value\":" + num(r.value) +
		",\"left\":" + limitJson(r.left) + ",\"right\":" + limitJson(r.right) +
		",\"kind\":" + str(r.kindId) + ",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::tangent(const std::string& expression, double point) {
	const auto r = DerivativeAtPoint::analyze(expression, point);
	const auto rows = [](const std::vector<DifferenceQuotientRow>& list) {
		std::string out = "[";
		for (std::size_t i = 0; i < list.size(); ++i) {
			if (i) out += ",";
			out += "{\"h\":" + num(list[i].h) + ",\"x\":" + num(list[i].x) + ",\"slope\":" +
				(list[i].defined ? num(list[i].secantSlope) : "null") + "}";
		}
		return out + "]";
	};
	return "{\"expression\":" + str(r.expression) + ",\"derivative\":" + str(r.derivativeExpression) +
		",\"point\":" + num(r.point) + ",\"valueDefined\":" + boolean(r.valueDefined) + ",\"value\":" + num(r.value) +
		",\"leftDerivative\":" + num(r.leftDerivative) + ",\"rightDerivative\":" + num(r.rightDerivative) +
		",\"differentiable\":" + boolean(r.differentiable) + ",\"slope\":" + num(r.slope) +
		",\"problem\":" + str(r.problem) + ",\"tangent\":" + str(r.tangentLine) + ",\"normal\":" + str(r.normalLine) +
		",\"rightRows\":" + rows(r.rightRows) + ",\"leftRows\":" + rows(r.leftRows) +
		",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::meanValue(const std::string& expression, double a, double b) {
	const auto r = DerivativeAtPoint::meanValue(expression, a, b);
	return "{\"expression\":" + str(r.expression) + ",\"a\":" + num(r.a) + ",\"b\":" + num(r.b) +
		",\"fa\":" + num(r.fa) + ",\"fb\":" + num(r.fb) + ",\"meanSlope\":" + num(r.meanSlope) +
		",\"continuous\":" + boolean(r.continuousOnInterval) + ",\"differentiable\":" + boolean(r.differentiableInInterior) +
		",\"lagrange\":" + boolean(r.lagrangeApplies) + ",\"rolle\":" + boolean(r.rolleApplies) +
		",\"points\":" + numbers(r.points) + ",\"pointValues\":" + numbers(r.pointValues) +
		",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::study(const std::string& expression, double xMin, double xMax) {
	const auto r = FunctionStudy::analyze(expression, xMin, xMax, 400);
	std::string critical = "[";
	for (std::size_t i = 0; i < r.criticalPoints.size(); ++i) {
		if (i) critical += ",";
		const auto& p = r.criticalPoints[i];
		critical += "{\"x\":" + num(p.x) + ",\"y\":" + num(p.y) + ",\"second\":" + num(p.secondDerivative) + ",\"kind\":" + str(p.kind) + "}";
	}
	critical += "]";
	std::string inflections = "[";
	for (std::size_t i = 0; i < r.inflectionPoints.size(); ++i) {
		if (i) inflections += ",";
		inflections += "{\"x\":" + num(r.inflectionPoints[i].x) + ",\"y\":" + num(r.inflectionPoints[i].y) + "}";
	}
	inflections += "]";
	std::string asymptotes = "[";
	for (std::size_t i = 0; i < r.asymptotes.size(); ++i) {
		if (i) asymptotes += ",";
		const auto& a = r.asymptotes[i];
		asymptotes += "{\"kind\":" + str(a.kind) + ",\"equation\":" + str(a.equation) + ",\"x\":" + num(a.x) +
			",\"slope\":" + num(a.slope) + ",\"offset\":" + num(a.offset) + ",\"side\":" + str(a.side) + "}";
	}
	asymptotes += "]";
	return "{\"expression\":" + str(r.expression) + ",\"firstDerivative\":" + str(r.firstDerivative) +
		",\"secondDerivative\":" + str(r.secondDerivative) + ",\"xMin\":" + num(r.xMin) + ",\"xMax\":" + num(r.xMax) +
		",\"f\":" + pointsJson(r.f) + ",\"df\":" + pointsJson(r.df) + ",\"ddf\":" + pointsJson(r.ddf) +
		",\"domain\":" + intervalsJson(r.domain) + ",\"zeros\":" + numbers(r.zeros) +
		",\"critical\":" + critical + ",\"inflections\":" + inflections +
		",\"monotonicity\":" + intervalsJson(r.monotonicity) + ",\"concavity\":" + intervalsJson(r.concavity) +
		",\"asymptotes\":" + asymptotes + ",\"steps\":" + steps(r.steps) + "}";
}

std::string DifferentialCalculusJson::sample(const std::string& expression, double xMin, double xMax, int count) {
	return pointsJson(FunctionStudy::sample(expression, xMin, xMax, count));
}

} // namespace aksiomat::differential_calculus
