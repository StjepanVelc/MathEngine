#include "aksiomat/logic/NormalForms.hpp"

#include <stdexcept>

#include "aksiomat/logic/TruthTable.hpp"

namespace aksiomat {
namespace {

using Ptr = LogicExpression::Ptr;

Ptr nnf(const Ptr& expression, bool negated) {
	switch (expression->op()) {
	case LogicOp::Variable:
		return negated ? LogicExpression::negation(expression) : expression;
	case LogicOp::Constant:
		return LogicExpression::constant(negated ? !expression->value() : expression->value());
	case LogicOp::Not:
		return nnf(expression->left(), !negated);
	case LogicOp::And:
		return negated
				   ? LogicExpression::disjunction(nnf(expression->left(), true), nnf(expression->right(), true))
				   : LogicExpression::conjunction(nnf(expression->left(), false), nnf(expression->right(), false));
	case LogicOp::Or:
		return negated
				   ? LogicExpression::conjunction(nnf(expression->left(), true), nnf(expression->right(), true))
				   : LogicExpression::disjunction(nnf(expression->left(), false), nnf(expression->right(), false));
	case LogicOp::Implies:
		return negated
				   ? LogicExpression::conjunction(nnf(expression->left(), false), nnf(expression->right(), true))
				   : LogicExpression::disjunction(nnf(expression->left(), true), nnf(expression->right(), false));
	case LogicOp::Iff:
		if (negated) {
			return LogicExpression::disjunction(
				LogicExpression::conjunction(nnf(expression->left(), false), nnf(expression->right(), true)),
				LogicExpression::conjunction(nnf(expression->left(), true), nnf(expression->right(), false)));
		}
		return LogicExpression::disjunction(
			LogicExpression::conjunction(nnf(expression->left(), false), nnf(expression->right(), false)),
			LogicExpression::conjunction(nnf(expression->left(), true), nnf(expression->right(), true)));
	}
	throw std::logic_error("Nepoznat logicki operator");
}

std::size_t nodeCount(const Ptr& expression) {
	switch (expression->op()) {
	case LogicOp::Variable:
	case LogicOp::Constant:
		return 1;
	case LogicOp::Not:
		return 1 + nodeCount(expression->left());
	default:
		return 1 + nodeCount(expression->left()) + nodeCount(expression->right());
	}
}

void enforceLimit(const Ptr& expression) {
	if (nodeCount(expression) > NormalForms::kMaxNodes) {
		throw std::invalid_argument("Normalna forma prelazi dopusteni limit velicine");
	}
}

Ptr distributeOr(const Ptr& lhs, const Ptr& rhs) {
	if (lhs->op() == LogicOp::And) {
		auto result = LogicExpression::conjunction(distributeOr(lhs->left(), rhs),
												   distributeOr(lhs->right(), rhs));
		enforceLimit(result);
		return result;
	}
	if (rhs->op() == LogicOp::And) {
		auto result = LogicExpression::conjunction(distributeOr(lhs, rhs->left()),
												   distributeOr(lhs, rhs->right()));
		enforceLimit(result);
		return result;
	}
	return LogicExpression::disjunction(lhs, rhs);
}

Ptr distributeAnd(const Ptr& lhs, const Ptr& rhs) {
	if (lhs->op() == LogicOp::Or) {
		auto result = LogicExpression::disjunction(distributeAnd(lhs->left(), rhs),
												   distributeAnd(lhs->right(), rhs));
		enforceLimit(result);
		return result;
	}
	if (rhs->op() == LogicOp::Or) {
		auto result = LogicExpression::disjunction(distributeAnd(lhs, rhs->left()),
												   distributeAnd(lhs, rhs->right()));
		enforceLimit(result);
		return result;
	}
	return LogicExpression::conjunction(lhs, rhs);
}

Ptr cnf(const Ptr& expression) {
	if (expression->op() == LogicOp::And) {
		return LogicExpression::conjunction(cnf(expression->left()), cnf(expression->right()));
	}
	if (expression->op() == LogicOp::Or) {
		return distributeOr(cnf(expression->left()), cnf(expression->right()));
	}
	return expression;
}

Ptr dnf(const Ptr& expression) {
	if (expression->op() == LogicOp::Or) {
		return LogicExpression::disjunction(dnf(expression->left()), dnf(expression->right()));
	}
	if (expression->op() == LogicOp::And) {
		return distributeAnd(dnf(expression->left()), dnf(expression->right()));
	}
	return expression;
}

Ptr combine(const std::vector<Ptr>& expressions, bool conjunction) {
	if (expressions.empty()) return LogicExpression::constant(conjunction);
	auto result = expressions.front();
	for (std::size_t i = 1; i < expressions.size(); ++i) {
		result = conjunction ? LogicExpression::conjunction(result, expressions[i])
							 : LogicExpression::disjunction(result, expressions[i]);
		enforceLimit(result);
	}
	return result;
}

} // namespace

LogicExpression::Ptr NormalForms::toNnf(const LogicExpression::Ptr& expression) {
	auto result = nnf(expression, false);
	enforceLimit(result);
	return result;
}

LogicExpression::Ptr NormalForms::toCnf(const LogicExpression::Ptr& expression) {
	auto result = cnf(toNnf(expression));
	enforceLimit(result);
	return result;
}

LogicExpression::Ptr NormalForms::toDnf(const LogicExpression::Ptr& expression) {
	auto result = dnf(toNnf(expression));
	enforceLimit(result);
	return result;
}

LogicExpression::Ptr NormalForms::toCanonicalCnf(const LogicExpression::Ptr& expression) {
	const auto table = TruthTable::generate(expression);
	std::vector<Ptr> clauses;
	for (const auto& row : table.rows) {
		if (row.result) continue;
		std::vector<Ptr> literals;
		for (const auto& variable : table.variables) {
			auto literal = LogicExpression::variable(variable);
			// Maks-term mora biti neistinit u ovom retku.
			if (row.valuation.at(variable)) literal = LogicExpression::negation(literal);
			literals.push_back(std::move(literal));
		}
		clauses.push_back(combine(literals, false));
	}
	auto result = combine(clauses, true);
	enforceLimit(result);
	return result;
}

LogicExpression::Ptr NormalForms::toCanonicalDnf(const LogicExpression::Ptr& expression) {
	const auto table = TruthTable::generate(expression);
	std::vector<Ptr> terms;
	for (const auto& row : table.rows) {
		if (!row.result) continue;
		std::vector<Ptr> literals;
		for (const auto& variable : table.variables) {
			auto literal = LogicExpression::variable(variable);
			// Min-term mora biti istinit u ovom retku.
			if (!row.valuation.at(variable)) literal = LogicExpression::negation(literal);
			literals.push_back(std::move(literal));
		}
		terms.push_back(combine(literals, true));
	}
	auto result = combine(terms, false);
	enforceLimit(result);
	return result;
}

} // namespace aksiomat
