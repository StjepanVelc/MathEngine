#include "aksiomat/algebra/EquationSolver.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/AlgebraParser.hpp"
#include "aksiomat/algebra/LinearForm.hpp"

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

std::string number(double value) {
	if (std::abs(value) < epsilon) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

} // namespace

EquationSolution EquationSolver::solve(const std::string& equation, const std::string& variable) {
	const auto separator = equation.find('=');
	if (separator == std::string::npos || equation.find('=', separator + 1) != std::string::npos) {
		throw std::invalid_argument("Jednadzba mora sadrzavati tocno jedan znak '='");
	}
	const auto lhs = extractLinearForm(AlgebraParser::parse(equation.substr(0, separator)), variable);
	const auto rhs = extractLinearForm(AlgebraParser::parse(equation.substr(separator + 1)), variable);
	const double coefficient = lhs.coefficient - rhs.coefficient;
	const double constant = rhs.constant - lhs.constant;

	EquationSolution solution;
	solution.variable = variable;
	solution.steps.push_back(formatLinearForm(lhs, variable) + " = " + formatLinearForm(rhs, variable));
	solution.steps.push_back(number(coefficient) + variable + " = " + number(constant));
	if (std::abs(coefficient) < epsilon) {
		if (std::abs(constant) < epsilon) {
			solution.type = EquationSolutionType::Infinite;
			solution.steps.push_back("0 = 0, jednadzba vrijedi za svaki realan broj");
		} else {
			solution.type = EquationSolutionType::None;
			solution.steps.push_back("0 = " + number(constant) + ", jednadzba nema rjesenja");
		}
		return solution;
	}
	solution.type = EquationSolutionType::Unique;
	solution.value = constant / coefficient;
	solution.steps.push_back(variable + " = " + number(constant) + " / " + number(coefficient));
	solution.steps.push_back(variable + " = " + number(solution.value));
	return solution;
}

} // namespace aksiomat::algebra
