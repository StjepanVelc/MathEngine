#include "aksiomat/geometry/UnitConversion.hpp"

#include <array>
#include <cmath>
#include <stdexcept>

namespace aksiomat::geometry {
namespace {

void validateValue(double value) {
	if (!std::isfinite(value) || value < 0.0) {
		throw std::invalid_argument("Mjera mora biti konacan nenegativan broj");
	}
}

template <typename Unit, std::size_t Size>
double convert(double value, Unit from, Unit to, const std::array<double, Size>& factors) {
	validateValue(value);
	const auto fromIndex = static_cast<std::size_t>(from);
	const auto toIndex = static_cast<std::size_t>(to);
	if (fromIndex >= factors.size() || toIndex >= factors.size()) {
		throw std::invalid_argument("Nepoznata mjerna jedinica");
	}
	return value * factors[fromIndex] / factors[toIndex];
}

} // namespace

double UnitConversion::length(double value, LengthUnit from, LengthUnit to) {
	constexpr std::array factors{0.001, 0.01, 0.1, 1.0, 1000.0};
	return convert(value, from, to, factors);
}

double UnitConversion::area(double value, AreaUnit from, AreaUnit to) {
	constexpr std::array factors{0.000001, 0.0001, 0.01, 1.0, 10000.0, 1000000.0};
	return convert(value, from, to, factors);
}

double UnitConversion::volume(double value, VolumeUnit from, VolumeUnit to) {
	constexpr std::array factors{0.000000001, 0.000001, 0.001, 1.0, 0.000001, 0.001};
	return convert(value, from, to, factors);
}

} // namespace aksiomat::geometry
