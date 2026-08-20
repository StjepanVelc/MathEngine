#pragma once

#include <string>
#include <vector>

namespace aksiomat::algebra {

enum class LinearSystemSolutionType { Unique, Infinite, None };

struct LinearSystemSolution {
	LinearSystemSolutionType type = LinearSystemSolutionType::None;
	double x = 0.0;
	double y = 0.0;
	std::vector<std::string> steps;
	std::vector<std::string> cramerSteps;
	std::vector<std::string> substitutionSteps;
	std::vector<std::string> eliminationSteps;
};

class LinearSystemSolver {
public:
	static LinearSystemSolution solve(const std::string& firstEquation, const std::string& secondEquation);
};

} // namespace aksiomat::algebra
