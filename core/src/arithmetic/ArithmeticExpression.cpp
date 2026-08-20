#include "aksiomat/arithmetic/ArithmeticExpression.hpp"

#include <utility>

namespace aksiomat {

ArithmeticExpression::ArithmeticExpression(ArithmeticOp op, double value, std::string name,
										   std::vector<Ptr> arguments)
	: op_(op), value_(value), name_(std::move(name)), arguments_(std::move(arguments)) {}

ArithmeticExpression::Ptr ArithmeticExpression::number(double value) {
	return Ptr(new ArithmeticExpression(ArithmeticOp::Number, value, "", {}));
}

ArithmeticExpression::Ptr ArithmeticExpression::constant(std::string name) {
	return Ptr(new ArithmeticExpression(ArithmeticOp::Constant, 0.0, std::move(name), {}));
}

ArithmeticExpression::Ptr ArithmeticExpression::unary(ArithmeticOp op, Ptr operand) {
	return Ptr(new ArithmeticExpression(op, 0.0, "", {std::move(operand)}));
}

ArithmeticExpression::Ptr ArithmeticExpression::binary(ArithmeticOp op, Ptr lhs, Ptr rhs) {
	return Ptr(new ArithmeticExpression(op, 0.0, "", {std::move(lhs), std::move(rhs)}));
}

ArithmeticExpression::Ptr ArithmeticExpression::function(std::string name,
														  std::vector<Ptr> arguments) {
	return Ptr(new ArithmeticExpression(ArithmeticOp::Function, 0.0, std::move(name),
										std::move(arguments)));
}

} // namespace aksiomat
