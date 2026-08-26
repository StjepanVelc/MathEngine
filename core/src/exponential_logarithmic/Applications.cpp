#include "aksiomat/exponential_logarithmic/Applications.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::exponential_logarithmic {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Value must be finite");
}

void requirePositive(double value, const char* message) {
	requireFinite(value);
	if (value <= 0.0) throw std::invalid_argument(message);
}

} // namespace

ApplicationResult Applications::radioactiveDecay(double initialAmount, double halfLifePeriod, double elapsedTime) {
	requirePositive(initialAmount, "Initial amount must be positive");
	requirePositive(halfLifePeriod, "Half-life period must be positive");
	requireFinite(elapsedTime);
	if (elapsedTime < 0.0) throw std::invalid_argument("Elapsed time cannot be negative");
	const double value = initialAmount * std::pow(0.5, elapsedTime / halfLifePeriod);
	return {ApplicationKind::RadioactiveDecay, value};
}

ApplicationResult Applications::phLevel(double hydrogenIonConcentration) {
	requirePositive(hydrogenIonConcentration, "Hydrogen ion concentration must be positive");
	const double value = -std::log10(hydrogenIonConcentration);
	return {ApplicationKind::PhLevel, value};
}

ApplicationResult Applications::richterMagnitude(double waveAmplitudeRatio) {
	requirePositive(waveAmplitudeRatio, "Wave amplitude ratio must be positive");
	const double value = std::log10(waveAmplitudeRatio);
	return {ApplicationKind::RichterMagnitude, value};
}

ApplicationResult Applications::soundIntensityDecibels(double intensity, double referenceIntensity) {
	requirePositive(intensity, "Intensity must be positive");
	requirePositive(referenceIntensity, "Reference intensity must be positive");
	const double value = 10.0 * std::log10(intensity / referenceIntensity);
	return {ApplicationKind::SoundIntensityDecibels, value};
}

} // namespace aksiomat::exponential_logarithmic
