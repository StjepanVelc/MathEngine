#pragma once

namespace aksiomat::exponential_logarithmic {

enum class ApplicationKind {
	RadioactiveDecay,
	PhLevel,
	RichterMagnitude,
	SoundIntensityDecibels
};

struct ApplicationResult {
	ApplicationKind kind;
	double value;
};

class Applications {
public:
	static ApplicationResult radioactiveDecay(double initialAmount, double halfLifePeriod, double elapsedTime);
	static ApplicationResult phLevel(double hydrogenIonConcentration);
	static ApplicationResult richterMagnitude(double waveAmplitudeRatio);
	static ApplicationResult soundIntensityDecibels(double intensity, double referenceIntensity);
};

} // namespace aksiomat::exponential_logarithmic
