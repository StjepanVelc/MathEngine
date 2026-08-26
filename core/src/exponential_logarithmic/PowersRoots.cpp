#include "aksiomat/exponential_logarithmic/PowersRoots.hpp"

#include <cmath>
#include <numeric>
#include <stdexcept>

namespace aksiomat::exponential_logarithmic {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Value must be finite");
}

} // namespace

PowerResult PowersRoots::power(double base, double exponent) {
	requireFinite(base); requireFinite(exponent);
	if (base == 0.0 && exponent < 0.0) throw std::invalid_argument("Cannot raise zero to a negative exponent");
	if (base < 0.0 && std::floor(exponent) != exponent) throw std::invalid_argument("Negative base requires an integer exponent");
	const double value = std::pow(base, exponent);
	if (!std::isfinite(value)) throw std::invalid_argument("Power result is not finite");
	return {base, exponent, value};
}

RootResult PowersRoots::root(double radicand, double degree) {
	requireFinite(radicand); requireFinite(degree);
	if (degree == 0.0) throw std::invalid_argument("Root degree cannot be zero");
	if (radicand < 0.0) {
		const bool isOddDegree = std::floor(degree) == degree && (static_cast<long long>(degree) % 2 != 0);
		if (!isOddDegree) throw std::invalid_argument("Even root of a negative number is not real");
		const double value = -std::pow(-radicand, 1.0 / degree);
		return {radicand, degree, value};
	}
	return {radicand, degree, std::pow(radicand, 1.0 / degree)};
}

double PowersRoots::nthRoot(double radicand, double degree) {
	return root(radicand, degree).value;
}

double PowersRoots::rationalPower(double base, long numerator, long denominator) {
	if (denominator == 0) throw std::invalid_argument("Denominator cannot be zero");
	requireFinite(base);
	const RootResult rooted = root(base, static_cast<double>(denominator));
	return power(rooted.value, static_cast<double>(numerator)).value;
}

bool PowersRoots::isPerfectSquare(long long value) {
	if (value < 0) return false;
	const long long root = static_cast<long long>(std::llround(std::sqrt(static_cast<double>(value))));
	for (long long candidate = std::max<long long>(0, root - 1); candidate <= root + 1; ++candidate) {
		if (candidate * candidate == value) return true;
	}
	return false;
}

} // namespace aksiomat::exponential_logarithmic
