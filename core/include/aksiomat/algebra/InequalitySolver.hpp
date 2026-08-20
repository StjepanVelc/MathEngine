#pragma once

#include <string>
#include <vector>

namespace aksiomat::algebra {

enum class InequalitySolutionType { Interval, AllReal, Empty };

struct InequalitySolution {
	InequalitySolutionType type = InequalitySolutionType::Empty;
	double boundary = 0.0;
	std::string relation;
	std::string interval;
	std::vector<std::string> steps;
};

class InequalitySolver {
public:
	static InequalitySolution solve(const std::string& inequality, const std::string& variable = "x");
};

} // namespace aksiomat::algebra
