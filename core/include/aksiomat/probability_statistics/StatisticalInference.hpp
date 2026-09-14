#pragma once

#include <string>
#include <vector>

namespace aksiomat::probability_statistics {

struct ConfidenceIntervalResult {
	double sampleMean;
	double marginOfError;
	double lowerBound;
	double upperBound;
	std::vector<std::string> steps;
};

struct HypothesisTestResult {
	double testStatistic;
	double criticalValue;
	bool rejectNull;
	std::vector<std::string> steps;
};

struct RegressionResult {
	double slope;
	double intercept;
	double correlation;
	double rSquared;
	std::vector<std::string> steps;
};

// Statisticko zakljucivanje: procjena parametara, testiranje hipoteza i regresija.
class StatisticalInference {
public:
	// Interval pouzdanosti za sredinu populacije koristeci z-vrijednost (poznata sigma) ili t-aproksimaciju.
	static ConfidenceIntervalResult confidenceIntervalForMean(const std::vector<double>& sample, double confidenceLevel, double populationStdDev);

	// Jednostavni z-test o sredini populacije uz poznatu standardnu devijaciju.
	static HypothesisTestResult zTestForMean(const std::vector<double>& sample, double hypothesizedMean, double populationStdDev, double significanceLevel);

	// Jednostavna linearna regresija metodom najmanjih kvadrata.
	static RegressionResult simpleLinearRegression(const std::vector<double>& x, const std::vector<double>& y);
};

} // namespace aksiomat::probability_statistics
