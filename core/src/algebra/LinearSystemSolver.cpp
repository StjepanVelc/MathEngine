#include "aksiomat/algebra/LinearSystemSolver.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/AlgebraParser.hpp"

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

struct Row {
	double x;
	double y;
	double value;
};

struct TwoVariableForm {
	double x = 0.0;
	double y = 0.0;
	double constant = 0.0;
};

TwoVariableForm extract(const AlgebraExpression::Ptr& expression) {
	switch (expression->op()) {
	case AlgebraOp::Number: return {0.0, 0.0, expression->value()};
	case AlgebraOp::Variable:
		if (expression->name() == "x") return {1.0, 0.0, 0.0};
		if (expression->name() == "y") return {0.0, 1.0, 0.0};
		throw std::invalid_argument("Sustav podrzava samo varijable x i y");
	case AlgebraOp::Positive: return extract(expression->lhs());
	case AlgebraOp::Negate: {
		const auto value = extract(expression->lhs());
		return {-value.x, -value.y, -value.constant};
	}
	case AlgebraOp::Add:
	case AlgebraOp::Subtract: {
		const auto lhs = extract(expression->lhs());
		const auto rhs = extract(expression->rhs());
		const double sign = expression->op() == AlgebraOp::Add ? 1.0 : -1.0;
		return {lhs.x + sign * rhs.x, lhs.y + sign * rhs.y, lhs.constant + sign * rhs.constant};
	}
	case AlgebraOp::Multiply: {
		const auto lhs = extract(expression->lhs());
		const auto rhs = extract(expression->rhs());
		const bool lhsVariable = std::abs(lhs.x) >= epsilon || std::abs(lhs.y) >= epsilon;
		const bool rhsVariable = std::abs(rhs.x) >= epsilon || std::abs(rhs.y) >= epsilon;
		if (lhsVariable && rhsVariable) throw std::invalid_argument("Sustav mora biti linearan");
		if (lhsVariable) return {lhs.x * rhs.constant, lhs.y * rhs.constant, lhs.constant * rhs.constant};
		if (rhsVariable) return {rhs.x * lhs.constant, rhs.y * lhs.constant, rhs.constant * lhs.constant};
		return {0.0, 0.0, lhs.constant * rhs.constant};
	}
	case AlgebraOp::Divide: {
		const auto lhs = extract(expression->lhs());
		const auto rhs = extract(expression->rhs());
		if (std::abs(rhs.x) >= epsilon || std::abs(rhs.y) >= epsilon) {
			throw std::invalid_argument("Djelitelj ne smije sadrzavati varijablu");
		}
		if (std::abs(rhs.constant) < epsilon) throw std::invalid_argument("Dijeljenje nulom");
		return {lhs.x / rhs.constant, lhs.y / rhs.constant, lhs.constant / rhs.constant};
	}
	case AlgebraOp::Power:
		if (expression->rhs()->op() == AlgebraOp::Number &&
			std::abs(expression->rhs()->value() - 1.0) < epsilon) return extract(expression->lhs());
		throw std::invalid_argument("Sustav mora biti linearan");
	}
	throw std::invalid_argument("Nepodrzan izraz sustava");
}

std::string number(double value) {
	if (std::abs(value) < epsilon) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

std::string rowText(const Row& row) {
	return number(row.x) + "x + " + number(row.y) + "y = " + number(row.value);
}

Row parseEquation(const std::string& equation) {
	const auto separator = equation.find('=');
	if (separator == std::string::npos || equation.find('=', separator + 1) != std::string::npos) {
		throw std::invalid_argument("Svaka jednadzba mora sadrzavati tocno jedan znak '='");
	}
	const auto lhsExpression = AlgebraParser::parse(equation.substr(0, separator));
	const auto rhsExpression = AlgebraParser::parse(equation.substr(separator + 1));
	const auto lhs = extract(lhsExpression);
	const auto rhs = extract(rhsExpression);
	return {lhs.x - rhs.x, lhs.y - rhs.y, rhs.constant - lhs.constant};
}

bool rowIsZero(const Row& row) {
	return std::abs(row.x) < epsilon && std::abs(row.y) < epsilon;
}

} // namespace

LinearSystemSolution LinearSystemSolver::solve(const std::string& firstEquation, const std::string& secondEquation) {
	const auto first = parseEquation(firstEquation);
	const auto second = parseEquation(secondEquation);
	const double determinant = first.x * second.y - second.x * first.y;
	const double determinantX = first.value * second.y - second.value * first.y;
	const double determinantY = first.x * second.value - second.x * first.value;

	LinearSystemSolution solution;
	solution.steps.push_back(rowText(first));
	solution.steps.push_back(rowText(second));
	solution.cramerSteps = solution.steps;
	solution.cramerSteps.push_back("D = " + number(determinant) + ", Dx = " + number(determinantX) + ", Dy = " + number(determinantY));
	if (std::abs(determinant) >= epsilon) {
		solution.type = LinearSystemSolutionType::Unique;
		solution.x = determinantX / determinant;
		solution.y = determinantY / determinant;
		solution.cramerSteps.push_back("x = Dx / D = " + number(solution.x));
		solution.cramerSteps.push_back("y = Dy / D = " + number(solution.y));
		solution.steps = solution.cramerSteps;

		const bool useFirstX = std::abs(first.x) >= epsilon;
		const Row& source = useFirstX ? first : second;
		const Row& target = useFirstX ? second : first;
		if (useFirstX || std::abs(source.y) >= epsilon) {
			if (std::abs(source.x) >= epsilon) {
				solution.substitutionSteps.push_back("Izoliraj x: x = (" + number(source.value) + " - " +
					number(source.y) + "y) / " + number(source.x));
				solution.substitutionSteps.push_back("Uvrsti u drugu jednadzbu i dobij y = " + number(solution.y));
				solution.substitutionSteps.push_back("Vrati y u izolirani izraz: x = " + number(solution.x));
			} else {
				solution.substitutionSteps.push_back("Izoliraj y: y = " + number(source.value / source.y));
				solution.substitutionSteps.push_back("Uvrsti y u drugu jednadzbu: x = " + number(solution.x));
			}
		}

		const double firstMultiplier = second.x;
		const double secondMultiplier = -first.x;
		solution.eliminationSteps.push_back("Pomnozi prvu jednadzbu s " + number(firstMultiplier) +
			", a drugu s " + number(secondMultiplier));
		solution.eliminationSteps.push_back("Zbrajanjem se uklanja x i dobiva y = " + number(solution.y));
		solution.eliminationSteps.push_back("Uvrstavanjem dobivamo x = " + number(solution.x));
		(void)target;
		return solution;
	}

	const bool inconsistentZeroRow = (rowIsZero(first) && std::abs(first.value) >= epsilon) ||
		(rowIsZero(second) && std::abs(second.value) >= epsilon);
	if (inconsistentZeroRow || std::abs(determinantX) >= epsilon || std::abs(determinantY) >= epsilon) {
		solution.type = LinearSystemSolutionType::None;
		solution.steps.push_back("Pravci su paralelni i sustav nema rjesenja");
	} else {
		solution.type = LinearSystemSolutionType::Infinite;
		solution.steps.push_back("Jednadzbe opisuju isti pravac pa sustav ima beskonacno mnogo rjesenja");
	}
	solution.cramerSteps = solution.steps;
	solution.substitutionSteps = solution.steps;
	solution.eliminationSteps = solution.steps;
	return solution;
}

} // namespace aksiomat::algebra
