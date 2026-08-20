#pragma once

#include <memory>
#include <string>
#include <vector>

namespace aksiomat {

enum class PredicateOp {
	Predicate,
	Equality,
	NotEqual,
	Not,
	And,
	Or,
	Implies,
	Iff,
	ForAll,
	Exists
};

class PredicateExpression {
public:
	using Ptr = std::shared_ptr<const PredicateExpression>;

	static Ptr predicate(std::string name, std::vector<std::string> arguments);
	static Ptr equality(std::string lhs, std::string rhs);
	static Ptr notEqual(std::string lhs, std::string rhs);
	static Ptr negation(Ptr operand);
	static Ptr conjunction(Ptr lhs, Ptr rhs);
	static Ptr disjunction(Ptr lhs, Ptr rhs);
	static Ptr implication(Ptr lhs, Ptr rhs);
	static Ptr equivalence(Ptr lhs, Ptr rhs);
	static Ptr forAll(std::string variable, Ptr body);
	static Ptr exists(std::string variable, Ptr body);

	PredicateOp op() const { return op_; }
	const std::string& name() const { return name_; }
	const std::vector<std::string>& arguments() const { return arguments_; }
	Ptr left() const { return left_; }
	Ptr right() const { return right_; }

	std::string toString() const;

private:
	PredicateExpression(PredicateOp op, std::string name,
						std::vector<std::string> arguments, Ptr left, Ptr right);

	PredicateOp op_;
	std::string name_;
	std::vector<std::string> arguments_;
	Ptr left_;
	Ptr right_;
};

} // namespace aksiomat
