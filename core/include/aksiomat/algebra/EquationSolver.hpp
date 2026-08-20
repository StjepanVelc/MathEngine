#pragma once

#include <string>
#include <vector>

namespace aksiomat::algebra {

enum class EquationSolutionType { Unique, Infinite, None };

struct EquationSolution {
	EquationSolutionType type = EquationSolutionType::None;
	double value = 0.0;
	std::string variable;
	std::vector<std::string> steps;
};

class EquationSolver {
public:
	static EquationSolution solve(const std::string& equation, const std::string& variable = "x");
};

} // namespace aksiomat::algebra
