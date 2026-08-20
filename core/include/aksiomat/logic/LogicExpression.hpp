#pragma once

#include <map>
#include <memory>
#include <string>

namespace aksiomat {

// Vrsta čvora u stablu logičkog izraza.
enum class LogicOp {
	Variable, // npr. "p", "q"
	Constant, // true / false
	Not,
	And,
	Or,
	Implies,
	Iff
};

// Nepromjenjivi AST čvor logičkog izraza.
class LogicExpression {
public:
	using Ptr = std::shared_ptr<const LogicExpression>;
	using Valuation = std::map<std::string, bool>;

	// Factory funkcije
	static Ptr variable(std::string name);
	static Ptr constant(bool value);
	static Ptr negation(Ptr operand);
	static Ptr conjunction(Ptr lhs, Ptr rhs);
	static Ptr disjunction(Ptr lhs, Ptr rhs);
	static Ptr implication(Ptr lhs, Ptr rhs);
	static Ptr equivalence(Ptr lhs, Ptr rhs);

	LogicOp op() const { return op_; }
	const std::string& name() const { return name_; }
	bool value() const { return value_; }
	Ptr left() const { return left_; }
	Ptr right() const { return right_; }

	// Evaluira izraz uz zadanu valuaciju varijabli.
	// Baca std::out_of_range ako varijabla nema pridruženu vrijednost.
	bool evaluate(const Valuation& valuation) const;

	// Tekstualni prikaz izraza, npr. "(p ∧ ¬q)".
	std::string toString() const;

private:
	LogicExpression(LogicOp op, std::string name, bool value, Ptr left, Ptr right);

	LogicOp op_;
	std::string name_;
	bool value_ = false;
	Ptr left_;
	Ptr right_;
};

} // namespace aksiomat
