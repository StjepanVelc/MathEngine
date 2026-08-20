#pragma once

#include <memory>
#include <string>
#include <vector>

namespace aksiomat {

enum class ArithmeticOp {
	Number,
	Constant,
	Add,
	Subtract,
	Multiply,
	Divide,
	Modulo,
	Power,
	Negate,
	Positive,
	Factorial,
	Function
};

class ArithmeticExpression {
public:
	using Ptr = std::shared_ptr<const ArithmeticExpression>;

	static Ptr number(double value);
	static Ptr constant(std::string name);
	static Ptr unary(ArithmeticOp op, Ptr operand);
	static Ptr binary(ArithmeticOp op, Ptr lhs, Ptr rhs);
	static Ptr function(std::string name, std::vector<Ptr> arguments);

	ArithmeticOp op() const { return op_; }
	double value() const { return value_; }
	const std::string& name() const { return name_; }
	const std::vector<Ptr>& arguments() const { return arguments_; }

private:
	ArithmeticExpression(ArithmeticOp op, double value, std::string name,
						 std::vector<Ptr> arguments);

	ArithmeticOp op_;
	double value_ = 0.0;
	std::string name_;
	std::vector<Ptr> arguments_;
};

} // namespace aksiomat
