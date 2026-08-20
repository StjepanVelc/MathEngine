#include "aksiomat/algebra/AlgebraExpression.hpp"

#include <stdexcept>
#include <utility>

namespace aksiomat::algebra {

AlgebraExpression::AlgebraExpression(AlgebraOp op, double value, std::string name, Ptr lhs, Ptr rhs)
	: op_(op), value_(value), name_(std::move(name)), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

AlgebraExpression::Ptr AlgebraExpression::number(double value) {
	return Ptr(new AlgebraExpression(AlgebraOp::Number, value, {}, nullptr, nullptr));
}

AlgebraExpression::Ptr AlgebraExpression::variable(std::string name) {
	if (name.empty()) throw std::invalid_argument("Naziv varijable ne smije biti prazan");
	return Ptr(new AlgebraExpression(AlgebraOp::Variable, 0.0, std::move(name), nullptr, nullptr));
}

AlgebraExpression::Ptr AlgebraExpression::unary(AlgebraOp op, Ptr operand) {
	if (op != AlgebraOp::Negate && op != AlgebraOp::Positive) {
		throw std::invalid_argument("Operator nije unaran");
	}
	if (!operand) throw std::invalid_argument("Nedostaje operand");
	return Ptr(new AlgebraExpression(op, 0.0, {}, std::move(operand), nullptr));
}

AlgebraExpression::Ptr AlgebraExpression::binary(AlgebraOp op, Ptr lhs, Ptr rhs) {
	if (op != AlgebraOp::Add && op != AlgebraOp::Subtract && op != AlgebraOp::Multiply &&
		op != AlgebraOp::Divide && op != AlgebraOp::Power) {
		throw std::invalid_argument("Operator nije binaran");
	}
	if (!lhs || !rhs) throw std::invalid_argument("Nedostaje operand");
	return Ptr(new AlgebraExpression(op, 0.0, {}, std::move(lhs), std::move(rhs)));
}

} // namespace aksiomat::algebra
