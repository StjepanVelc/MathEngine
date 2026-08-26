#include "aksiomat/exponential_logarithmic/Logarithms.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::exponential_logarithmic {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Value must be finite");
}

void requireValidBase(double base) {
	requireFinite(base);
	if (base <= 0.0 || base == 1.0) throw std::invalid_argument("Logarithm base must be positive and not equal to one");
}

void requirePositiveArgument(double argument) {
	requireFinite(argument);
	if (argument <= 0.0) throw std::invalid_argument("Logarithm argument must be positive");
}

} // namespace

double Logarithms::log(double argument, double base) {
	requirePositiveArgument(argument);
	requireValidBase(base);
	return std::log(argument) / std::log(base);
}

double Logarithms::naturalLog(double argument) {
	requirePositiveArgument(argument);
	return std::log(argument);
}

double Logarithms::commonLog(double argument) {
	requirePositiveArgument(argument);
	return std::log10(argument);
}

double Logarithms::binaryLog(double argument) {
	requirePositiveArgument(argument);
	return std::log2(argument);
}

double Logarithms::changeOfBase(double argument, double fromBase, double toBase) {
	requirePositiveArgument(argument);
	requireValidBase(fromBase);
	requireValidBase(toBase);
	return std::log(argument) / std::log(fromBase) * (std::log(fromBase) / std::log(toBase));
}

LogarithmicAnalysis Logarithms::analyze(LogarithmicFunction function, double xMin, double xMax, std::size_t sampleCount) {
	requireValidBase(function.base);
	requireFinite(xMin); requireFinite(xMax);
	if (xMin <= 0.0) throw std::invalid_argument("xMin must be positive for a logarithmic function");
	if (xMax <= xMin) throw std::invalid_argument("xMax must be greater than xMin");
	if (sampleCount < 2 || sampleCount > 10000) throw std::invalid_argument("Sample count must be between 2 and 10000");

	std::vector<LogarithmicSample> samples;
	samples.reserve(sampleCount);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double x = xMin + (xMax - xMin) * static_cast<double>(index) / static_cast<double>(sampleCount - 1);
		samples.push_back({x, log(x, function.base)});
	}

	return {function, 0.0, std::move(samples)};
}

} // namespace aksiomat::exponential_logarithmic
