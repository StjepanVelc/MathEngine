#pragma once

#include <optional>
#include <string>
#include <vector>

namespace aksiomat::algebra {

struct FunctionPoint {
	double x;
	double y;
};

struct FunctionAnalysis {
	std::string normalized;
	std::string domain;
	int degree = 0;
	FunctionPoint yIntercept;
	std::vector<FunctionPoint> xIntercepts;
	std::optional<FunctionPoint> vertex;
	std::string behavior;
	std::vector<FunctionPoint> samples;
};

class FunctionAnalyzer {
public:
	static FunctionAnalysis analyze(const std::string& expression, double minX = -10.0,
		double maxX = 10.0, std::size_t sampleCount = 401);
};

} // namespace aksiomat::algebra
