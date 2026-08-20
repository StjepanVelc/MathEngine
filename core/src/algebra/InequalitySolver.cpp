#include "aksiomat/algebra/InequalitySolver.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/AlgebraParser.hpp"
#include "aksiomat/algebra/LinearForm.hpp"

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

struct RelationPosition {
	std::size_t position;
	std::string relation;
};

RelationPosition findRelation(const std::string& input) {
	for (const auto& relation : {std::string("<="), std::string(">="), std::string("<"), std::string(">")}) {
		const auto position = input.find(relation);
		if (position != std::string::npos) return {position, relation};
	}
	throw std::invalid_argument("Nejednadzba mora sadrzavati <, <=, > ili >=");
}

std::string reverse(std::string relation) {
	if (relation == "<") return ">";
	if (relation == "<=") return ">=";
	if (relation == ">") return "<";
	return "<=";
}

std::string number(double value) {
	if (std::abs(value) < epsilon) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

bool compare(double lhs, const std::string& relation, double rhs) {
	if (relation == "<") return lhs < rhs;
	if (relation == "<=") return lhs <= rhs;
	if (relation == ">") return lhs > rhs;
	return lhs >= rhs;
}

std::string intervalFor(const std::string& relation, double boundary) {
	const auto value = number(boundary);
	if (relation == "<") return "(-inf, " + value + ")";
	if (relation == "<=") return "(-inf, " + value + "]";
	if (relation == ">") return "(" + value + ", inf)";
	return "[" + value + ", inf)";
}

} // namespace

InequalitySolution InequalitySolver::solve(const std::string& inequality, const std::string& variable) {
	const auto found = findRelation(inequality);
	const auto afterRelation = found.position + found.relation.size();
	for (std::size_t index = afterRelation; index < inequality.size(); ++index) {
		if (inequality[index] == '<' || inequality[index] == '>') {
			throw std::invalid_argument("Nejednadzba mora sadrzavati tocno jedan relacijski operator");
		}
	}
	const auto lhs = extractLinearForm(AlgebraParser::parse(inequality.substr(0, found.position)), variable);
	const auto rhs = extractLinearForm(AlgebraParser::parse(inequality.substr(afterRelation)), variable);
	const double coefficient = lhs.coefficient - rhs.coefficient;
	const double constant = rhs.constant - lhs.constant;

	InequalitySolution solution;
	solution.steps.push_back(formatLinearForm(lhs, variable) + " " + found.relation + " " + formatLinearForm(rhs, variable));
	std::string relation = found.relation;
	solution.steps.push_back(number(coefficient) + variable + " " + relation + " " + number(constant));
	if (std::abs(coefficient) < epsilon) {
		if (compare(0.0, relation, constant)) {
			solution.type = InequalitySolutionType::AllReal;
			solution.interval = "(-inf, inf)";
			solution.steps.push_back("Tvrdnja vrijedi za svaki realan broj");
		} else {
			solution.type = InequalitySolutionType::Empty;
			solution.interval = "prazan skup";
			solution.steps.push_back("Tvrdnja nije moguca");
		}
		return solution;
	}
	if (coefficient < 0.0) {
		relation = reverse(relation);
		solution.steps.push_back("Dijeljenjem negativnim brojem znak nejednakosti se obrce");
	}
	solution.type = InequalitySolutionType::Interval;
	solution.boundary = constant / coefficient;
	solution.relation = relation;
	solution.interval = intervalFor(relation, solution.boundary);
	solution.steps.push_back(variable + " " + relation + " " + number(solution.boundary));
	solution.steps.push_back("Interval rjesenja: " + solution.interval);
	return solution;
}

} // namespace aksiomat::algebra
