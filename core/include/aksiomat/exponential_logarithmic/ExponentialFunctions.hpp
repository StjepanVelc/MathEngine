#pragma once

#include <vector>

namespace aksiomat::exponential_logarithmic {

struct ExponentialFunction {
	double initialValue;
	double base;
};

struct ExponentialSample {
	double x;
	double y;
};

struct ExponentialAnalysis {
	ExponentialFunction function;
	bool isGrowth;
	double growthRatePercent;
	double horizontalAsymptote;
	std::vector<ExponentialSample> samples;
};

class ExponentialFunctions {
public:
	static double evaluate(ExponentialFunction function, double x);
	static ExponentialAnalysis analyze(ExponentialFunction function, double xMin, double xMax, std::size_t sampleCount);
	static double growthDecayValue(double initialValue, double ratePercent, double time);
	static double halfLife(double decayConstant);
	static double doublingTime(double growthRatePercent);
};

} // namespace aksiomat::exponential_logarithmic
