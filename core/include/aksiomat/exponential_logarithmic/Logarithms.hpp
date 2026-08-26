#pragma once

#include <vector>

namespace aksiomat::exponential_logarithmic {

struct LogarithmResult {
	double argument;
	double base;
	double value;
};

struct LogarithmicFunction {
	double base;
};

struct LogarithmicSample {
	double x;
	double y;
};

struct LogarithmicAnalysis {
	LogarithmicFunction function;
	double verticalAsymptoteX;
	std::vector<LogarithmicSample> samples;
};

class Logarithms {
public:
	static double log(double argument, double base);
	static double naturalLog(double argument);
	static double commonLog(double argument);
	static double binaryLog(double argument);
	static double changeOfBase(double argument, double fromBase, double toBase);
	static LogarithmicAnalysis analyze(LogarithmicFunction function, double xMin, double xMax, std::size_t sampleCount);
};

} // namespace aksiomat::exponential_logarithmic
