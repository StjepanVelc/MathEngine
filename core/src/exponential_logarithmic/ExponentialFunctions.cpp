#include "aksiomat/exponential_logarithmic/ExponentialFunctions.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::exponential_logarithmic {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Value must be finite");
}

ExponentialFunction requireFunction(ExponentialFunction function) {
	requireFinite(function.initialValue);
	requireFinite(function.base);
	if (function.base <= 0.0) throw std::invalid_argument("Exponential base must be positive");
	if (function.base == 1.0) throw std::invalid_argument("Exponential base cannot equal one");
	return function;
}

} // namespace

double ExponentialFunctions::evaluate(ExponentialFunction function, double x) {
	function = requireFunction(function);
	requireFinite(x);
	const double value = function.initialValue * std::pow(function.base, x);
	if (!std::isfinite(value)) throw std::invalid_argument("Exponential result is not finite");
	return value;
}

ExponentialAnalysis ExponentialFunctions::analyze(ExponentialFunction function, double xMin, double xMax, std::size_t sampleCount) {
	function = requireFunction(function);
	requireFinite(xMin); requireFinite(xMax);
	if (xMax <= xMin) throw std::invalid_argument("xMax must be greater than xMin");
	if (sampleCount < 2 || sampleCount > 10000) throw std::invalid_argument("Sample count must be between 2 and 10000");

	std::vector<ExponentialSample> samples;
	samples.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double x = xMin + (xMax - xMin) * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		samples.push_back({x, evaluate(function, x)});
	}

	const bool isGrowth = function.base > 1.0;
	const double growthRatePercent = (function.base - 1.0) * 100.0;
	return {function, isGrowth, growthRatePercent, 0.0, std::move(samples)};
}

double ExponentialFunctions::growthDecayValue(double initialValue, double ratePercent, double time) {
	requireFinite(initialValue); requireFinite(ratePercent); requireFinite(time);
	const double base = 1.0 + ratePercent / 100.0;
	if (base <= 0.0) throw std::invalid_argument("Resulting growth base must be positive");
	const double value = initialValue * std::pow(base, time);
	if (!std::isfinite(value)) throw std::invalid_argument("Growth/decay result is not finite");
	return value;
}

double ExponentialFunctions::halfLife(double decayConstant) {
	requireFinite(decayConstant);
	if (decayConstant <= 0.0) throw std::invalid_argument("Decay constant must be positive");
	return std::log(2.0) / decayConstant;
}

double ExponentialFunctions::doublingTime(double growthRatePercent) {
	requireFinite(growthRatePercent);
	if (growthRatePercent <= 0.0) throw std::invalid_argument("Growth rate must be positive");
	const double base = 1.0 + growthRatePercent / 100.0;
	return std::log(2.0) / std::log(base);
}

} // namespace aksiomat::exponential_logarithmic
