#pragma once

#include <string>
#include <vector>

namespace aksiomat::differential_calculus {

struct FunctionPoint {
	double x = 0.0;
	double y = 0.0;
	bool defined = false;
};

struct CriticalPoint {
	double x = 0.0;
	double y = 0.0;
	double secondDerivative = 0.0;
	// "maximum", "minimum" ili "stationary" (f'(x) = 0 bez ekstrema)
	std::string kind;
};

struct InflectionPoint {
	double x = 0.0;
	double y = 0.0;
};

struct IntervalBehavior {
	double from = 0.0;
	double to = 0.0;
	// monotonost: "increasing", "decreasing"; zakrivljenost: "convex", "concave"; domena: "defined"
	std::string behavior;
};

struct AsymptoteInfo {
	// "vertical", "horizontal", "slant"
	std::string kind;
	std::string equation;
	double slope = 0.0;
	double offset = 0.0;
	double x = 0.0;
	std::string side;
};

struct FunctionStudyResult {
	std::string expression;
	std::string firstDerivative;
	std::string secondDerivative;
	double xMin = 0.0;
	double xMax = 0.0;
	std::vector<FunctionPoint> f;
	std::vector<FunctionPoint> df;
	std::vector<FunctionPoint> ddf;
	std::vector<IntervalBehavior> domain;
	std::vector<double> zeros;
	std::vector<CriticalPoint> criticalPoints;
	std::vector<InflectionPoint> inflectionPoints;
	std::vector<IntervalBehavior> monotonicity;
	std::vector<IntervalBehavior> concavity;
	std::vector<AsymptoteInfo> asymptotes;
	std::vector<std::string> steps;
};

class FunctionStudy {
public:
	// Potpuno ispitivanje toka funkcije na [xMin, xMax]: domena, nultocke, f', f'', ekstremi, infleksije,
	// monotonost, zakrivljenost i asimptote, uz objasnjenje po koracima.
	static FunctionStudyResult analyze(const std::string& expression, double xMin, double xMax,
		int sampleCount = 400, const std::string& variable = "x");

	// Uzorci funkcije za crtanje grafa.
	static std::vector<FunctionPoint> sample(const std::string& expression, double xMin, double xMax,
		int sampleCount, const std::string& variable = "x");
};

} // namespace aksiomat::differential_calculus
