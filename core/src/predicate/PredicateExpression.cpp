#include "aksiomat/predicate/PredicateExpression.hpp"

#include <stdexcept>
#include <utility>

namespace aksiomat {

PredicateExpression::PredicateExpression(PredicateOp op, std::string name,
										 std::vector<std::string> arguments, Ptr left, Ptr right)
	: op_(op), name_(std::move(name)), arguments_(std::move(arguments)),
	  left_(std::move(left)), right_(std::move(right)) {}

PredicateExpression::Ptr PredicateExpression::predicate(std::string name,
														std::vector<std::string> arguments) {
	return Ptr(new PredicateExpression(PredicateOp::Predicate, std::move(name),
									   std::move(arguments), nullptr, nullptr));
}

PredicateExpression::Ptr PredicateExpression::equality(std::string lhs, std::string rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Equality, "", {std::move(lhs), std::move(rhs)},
									   nullptr, nullptr));
}

PredicateExpression::Ptr PredicateExpression::notEqual(std::string lhs, std::string rhs) {
	return Ptr(new PredicateExpression(PredicateOp::NotEqual, "", {std::move(lhs), std::move(rhs)},
									   nullptr, nullptr));
}

PredicateExpression::Ptr PredicateExpression::negation(Ptr operand) {
	return Ptr(new PredicateExpression(PredicateOp::Not, "", {}, std::move(operand), nullptr));
}

PredicateExpression::Ptr PredicateExpression::conjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::And, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::disjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Or, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::implication(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Implies, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::equivalence(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Iff, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::forAll(std::string variable, Ptr body) {
	return Ptr(new PredicateExpression(PredicateOp::ForAll, std::move(variable), {},
									   std::move(body), nullptr));
}

PredicateExpression::Ptr PredicateExpression::exists(std::string variable, Ptr body) {
	return Ptr(new PredicateExpression(PredicateOp::Exists, std::move(variable), {},
									   std::move(body), nullptr));
}

std::string PredicateExpression::toString() const {
	switch (op_) {
	case PredicateOp::Predicate: {
		std::string text = name_ + "(";
		for (std::size_t i = 0; i < arguments_.size(); ++i) {
			if (i > 0) text += ", ";
			text += arguments_[i];
		}
		return text + ")";
	}
	case PredicateOp::Equality:
		return arguments_[0] + " = " + arguments_[1];
	case PredicateOp::NotEqual:
		return arguments_[0] + " \u2260 " + arguments_[1];
	case PredicateOp::Not:
		return "\u00AC" + left_->toString();
	case PredicateOp::And:
		return "(" + left_->toString() + " \u2227 " + right_->toString() + ")";
	case PredicateOp::Or:
		return "(" + left_->toString() + " \u2228 " + right_->toString() + ")";
	case PredicateOp::Implies:
		return "(" + left_->toString() + " \u2192 " + right_->toString() + ")";
	case PredicateOp::Iff:
		return "(" + left_->toString() + " \u2194 " + right_->toString() + ")";
	case PredicateOp::ForAll:
		return "\u2200" + name_ + " " + left_->toString();
	case PredicateOp::Exists:
		return "\u2203" + name_ + " " + left_->toString();
	}
	throw std::logic_error("Nepoznat operator");
}

} // namespace aksiomat
