#include "aksiomat/algebra/AlgebraFormatter.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace aksiomat::algebra {
namespace {

int precedence(AlgebraOp op) {
	switch (op) {
	case AlgebraOp::Add:
	case AlgebraOp::Subtract: return 1;
	case AlgebraOp::Multiply:
	case AlgebraOp::Divide: return 2;
	case AlgebraOp::Negate:
	case AlgebraOp::Positive: return 3;
	case AlgebraOp::Power: return 4;
	default: return 5;
	}
}

std::string formatNumber(double value) {
	if (std::abs(value) < 1e-12) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

std::string formatNode(const AlgebraExpression::Ptr& expression, int parentPrecedence, bool rightChild) {
	if (!expression) throw std::invalid_argument("Nedostaje algebarski izraz");
	const auto op = expression->op();
	if (op == AlgebraOp::Number) return formatNumber(expression->value());
	if (op == AlgebraOp::Variable) return expression->name();

	const int current = precedence(op);
	std::string text;
	if (op == AlgebraOp::Negate || op == AlgebraOp::Positive) {
		text = (op == AlgebraOp::Negate ? "-" : "+") + formatNode(expression->lhs(), current, false);
	} else {
		const char* token = "";
		switch (op) {
		case AlgebraOp::Add: token = " + "; break;
		case AlgebraOp::Subtract: token = " - "; break;
		case AlgebraOp::Multiply: token = " * "; break;
		case AlgebraOp::Divide: token = " / "; break;
		case AlgebraOp::Power: token = "^"; break;
		default: throw std::invalid_argument("Nepoznat algebarski operator");
		}
		text = formatNode(expression->lhs(), current, false) + token +
			formatNode(expression->rhs(), current, op != AlgebraOp::Power);
	}

	const bool needsParentheses = current < parentPrecedence ||
		(rightChild && current == parentPrecedence &&
			(op == AlgebraOp::Add || op == AlgebraOp::Subtract || op == AlgebraOp::Multiply || op == AlgebraOp::Divide));
	return needsParentheses ? "(" + text + ")" : text;
}

} // namespace

std::string AlgebraFormatter::format(const AlgebraExpression::Ptr& expression) {
	return formatNode(expression, 0, false);
}

} // namespace aksiomat::algebra
