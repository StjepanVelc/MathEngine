#include "aksiomat/trigonometry/TrigFunctions.hpp"

#include <array>
#include <cmath>
#include <stdexcept>

namespace aksiomat::trigonometry {
namespace {

constexpr double valueTolerance = 1e-12;

struct ExactEntry {
	double degrees;
	const char* sine;
	const char* cosine;
	const char* tangent;
};

constexpr std::array exactEntries{
	ExactEntry{0.0, "0", "1", "0"},
	ExactEntry{30.0, "1/2", "sqrt(3)/2", "sqrt(3)/3"},
	ExactEntry{45.0, "sqrt(2)/2", "sqrt(2)/2", "1"},
	ExactEntry{60.0, "sqrt(3)/2", "1/2", "sqrt(3)"},
	ExactEntry{90.0, "1", "0", "undefined"},
	ExactEntry{120.0, "sqrt(3)/2", "-1/2", "-sqrt(3)"},
	ExactEntry{135.0, "sqrt(2)/2", "-sqrt(2)/2", "-1"},
	ExactEntry{150.0, "1/2", "-sqrt(3)/2", "-sqrt(3)/3"},
	ExactEntry{180.0, "0", "-1", "0"},
	ExactEntry{210.0, "-1/2", "-sqrt(3)/2", "sqrt(3)/3"},
	ExactEntry{225.0, "-sqrt(2)/2", "-sqrt(2)/2", "1"},
	ExactEntry{240.0, "-sqrt(3)/2", "-1/2", "sqrt(3)"},
	ExactEntry{270.0, "-1", "0", "undefined"},
	ExactEntry{300.0, "-sqrt(3)/2", "1/2", "-sqrt(3)"},
	ExactEntry{315.0, "-sqrt(2)/2", "sqrt(2)/2", "-1"},
	ExactEntry{330.0, "-1/2", "sqrt(3)/2", "-sqrt(3)/3"}
};

void requireUnit(AngleUnit unit) {
	if (unit != AngleUnit::Degrees && unit != AngleUnit::Radians) {
		throw std::invalid_argument("Nepoznata jedinica kuta");
	}
}

void requireFunction(TrigFunction function) {
	if (function != TrigFunction::Sine && function != TrigFunction::Cosine && function != TrigFunction::Tangent) {
		throw std::invalid_argument("Nepoznata trigonometrijska funkcija");
	}
}

const ExactEntry* findExact(double degrees) {
	for (const auto& entry : exactEntries) {
		if (std::abs(entry.degrees - degrees) < valueTolerance) return &entry;
	}
	return nullptr;
}

const char* exactFor(const ExactEntry& entry, TrigFunction function) {
	if (function == TrigFunction::Sine) return entry.sine;
	if (function == TrigFunction::Cosine) return entry.cosine;
	return entry.tangent;
}

} // namespace

TrigValue TrigFunctions::evaluate(TrigFunction function, double angle, AngleUnit unit) {
	requireFunction(function);
	requireUnit(unit);
	const auto point = Angles::unitCircle(angle, unit);
	if (function == TrigFunction::Tangent && std::abs(point.x) < valueTolerance) {
		throw std::invalid_argument("Tangens nije definiran kada je cos kuta jednak nuli");
	}
	double value = function == TrigFunction::Sine ? point.y :
		function == TrigFunction::Cosine ? point.x : point.y / point.x;
	if (std::abs(value) < valueTolerance) value = 0.0;
	if (const auto* entry = findExact(point.degrees)) {
		return {value, exactFor(*entry, function), true};
	}
	return {value, {}, false};
}

double TrigFunctions::inverse(TrigFunction function, double value, AngleUnit resultUnit) {
	requireFunction(function);
	requireUnit(resultUnit);
	if (!std::isfinite(value)) throw std::invalid_argument("Vrijednost mora biti konacan broj");
	if ((function == TrigFunction::Sine || function == TrigFunction::Cosine) && (value < -1.0 || value > 1.0)) {
		throw std::invalid_argument("Sinus i kosinus primaju vrijednosti od -1 do 1");
	}
	const double radians = function == TrigFunction::Sine ? std::asin(value) :
		function == TrigFunction::Cosine ? std::acos(value) : std::atan(value);
	return resultUnit == AngleUnit::Degrees ? Angles::toDegrees(radians) : radians;
}

} // namespace aksiomat::trigonometry
