#include "aksiomat/LogicExpression.hpp"

#include <stdexcept>
#include <utility>

namespace aksiomat {

LogicExpression::LogicExpression(LogicOp op, std::string name, bool value, Ptr left, Ptr right)
	: op_(op), name_(std::move(name)), value_(value), left_(std::move(left)), right_(std::move(right)) {}

LogicExpression::Ptr LogicExpression::variable(std::string name) {
	return Ptr(new LogicExpression(LogicOp::Variable, std::move(name), false, nullptr, nullptr));
}

LogicExpression::Ptr LogicExpression::constant(bool value) {
	return Ptr(new LogicExpression(LogicOp::Constant, {}, value, nullptr, nullptr));
}

LogicExpression::Ptr LogicExpression::negation(Ptr operand) {
	return Ptr(new LogicExpression(LogicOp::Not, {}, false, std::move(operand), nullptr));
}

LogicExpression::Ptr LogicExpression::conjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new LogicExpression(LogicOp::And, {}, false, std::move(lhs), std::move(rhs)));
}

LogicExpression::Ptr LogicExpression::disjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new LogicExpression(LogicOp::Or, {}, false, std::move(lhs), std::move(rhs)));
}

LogicExpression::Ptr LogicExpression::implication(Ptr lhs, Ptr rhs) {
	return Ptr(new LogicExpression(LogicOp::Implies, {}, false, std::move(lhs), std::move(rhs)));
}

LogicExpression::Ptr LogicExpression::equivalence(Ptr lhs, Ptr rhs) {
	return Ptr(new LogicExpression(LogicOp::Iff, {}, false, std::move(lhs), std::move(rhs)));
}

bool LogicExpression::evaluate(const Valuation& valuation) const {
	switch (op_) {
	case LogicOp::Variable:
		return valuation.at(name_);
	case LogicOp::Constant:
		return value_;
	case LogicOp::Not:
		return !left_->evaluate(valuation);
	case LogicOp::And:
		return left_->evaluate(valuation) && right_->evaluate(valuation);
	case LogicOp::Or:
		return left_->evaluate(valuation) || right_->evaluate(valuation);
	case LogicOp::Implies:
		return !left_->evaluate(valuation) || right_->evaluate(valuation);
	case LogicOp::Iff:
		return left_->evaluate(valuation) == right_->evaluate(valuation);
	}
	throw std::logic_error("Nepoznat logicki operator");
}

std::string LogicExpression::toString() const {
	switch (op_) {
	case LogicOp::Variable:
		return name_;
	case LogicOp::Constant:
		return value_ ? "T" : "F";
	case LogicOp::Not:
		return "\u00AC" + left_->toString();
	case LogicOp::And:
		return "(" + left_->toString() + " \u2227 " + right_->toString() + ")";
	case LogicOp::Or:
		return "(" + left_->toString() + " \u2228 " + right_->toString() + ")";
	case LogicOp::Implies:
		return "(" + left_->toString() + " \u2192 " + right_->toString() + ")";
	case LogicOp::Iff:
		return "(" + left_->toString() + " \u2194 " + right_->toString() + ")";
	}
	throw std::logic_error("Nepoznat logicki operator");
}

} // namespace aksiomat
