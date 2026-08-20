#pragma once

#include <memory>
#include <string>

namespace aksiomat::algebra {

enum class AlgebraOp {
	Number,
	Variable,
	Add,
	Subtract,
	Multiply,
	Divide,
	Power,
	Negate,
	Positive
};

class AlgebraExpression {
public:
	using Ptr = std::shared_ptr<const AlgebraExpression>;

	static Ptr number(double value);
	static Ptr variable(std::string name);
	static Ptr unary(AlgebraOp op, Ptr operand);
	static Ptr binary(AlgebraOp op, Ptr lhs, Ptr rhs);

	AlgebraOp op() const { return op_; }
	double value() const { return value_; }
	const std::string& name() const { return name_; }
	const Ptr& lhs() const { return lhs_; }
	const Ptr& rhs() const { return rhs_; }

private:
	AlgebraExpression(AlgebraOp op, double value, std::string name, Ptr lhs, Ptr rhs);

	AlgebraOp op_;
	double value_ = 0.0;
	std::string name_;
	Ptr lhs_;
	Ptr rhs_;
};

} // namespace aksiomat::algebra
