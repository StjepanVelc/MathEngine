#include "aksiomat/algebra/AlgebraSimplifier.hpp"

#include <cmath>
#include <map>
#include <stdexcept>
#include <utility>

#include "aksiomat/algebra/AlgebraFormatter.hpp"

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

bool isNumber(const AlgebraExpression::Ptr& expression, double value) {
	return expression->op() == AlgebraOp::Number && std::abs(expression->value() - value) < epsilon;
}

void flattenAdd(const AlgebraExpression::Ptr& expression, double sign,
	std::map<std::string, std::pair<double, AlgebraExpression::Ptr>>& terms, double& constant) {
	if (expression->op() == AlgebraOp::Add) {
		flattenAdd(expression->lhs(), sign, terms, constant);
		flattenAdd(expression->rhs(), sign, terms, constant);
		return;
	}
	if (expression->op() == AlgebraOp::Subtract) {
		flattenAdd(expression->lhs(), sign, terms, constant);
		flattenAdd(expression->rhs(), -sign, terms, constant);
		return;
	}
	if (expression->op() == AlgebraOp::Number) {
		constant += sign * expression->value();
		return;
	}

	double coefficient = sign;
	auto basis = expression;
	if (expression->op() == AlgebraOp::Negate) {
		coefficient = -coefficient;
		basis = expression->lhs();
	} else if (expression->op() == AlgebraOp::Multiply) {
		if (expression->lhs()->op() == AlgebraOp::Number) {
			coefficient *= expression->lhs()->value();
			basis = expression->rhs();
		} else if (expression->rhs()->op() == AlgebraOp::Number) {
			coefficient *= expression->rhs()->value();
			basis = expression->lhs();
		}
	}
	const auto key = AlgebraFormatter::format(basis);
	auto& item = terms[key];
	item.first += coefficient;
	item.second = basis;
}

AlgebraExpression::Ptr combineTerms(const AlgebraExpression::Ptr& expression) {
	if (expression->op() != AlgebraOp::Add && expression->op() != AlgebraOp::Subtract) return expression;
	std::map<std::string, std::pair<double, AlgebraExpression::Ptr>> terms;
	double constant = 0.0;
	flattenAdd(expression, 1.0, terms, constant);
	AlgebraExpression::Ptr result;
	auto append = [&](AlgebraExpression::Ptr term) {
		result = result ? AlgebraExpression::binary(AlgebraOp::Add, result, std::move(term)) : std::move(term);
	};
	for (const auto& [key, item] : terms) {
		(void)key;
		const double coefficient = item.first;
		if (std::abs(coefficient) < epsilon) continue;
		if (std::abs(coefficient - 1.0) < epsilon) append(item.second);
		else if (std::abs(coefficient + 1.0) < epsilon) append(AlgebraExpression::unary(AlgebraOp::Negate, item.second));
		else append(AlgebraExpression::binary(AlgebraOp::Multiply, AlgebraExpression::number(coefficient), item.second));
	}
	if (std::abs(constant) >= epsilon || !result) append(AlgebraExpression::number(constant));
	return result;
}

AlgebraExpression::Ptr simplifyNode(const AlgebraExpression::Ptr& expression) {
	if (!expression) throw std::invalid_argument("Nedostaje algebarski izraz");
	if (expression->op() == AlgebraOp::Number || expression->op() == AlgebraOp::Variable) return expression;
	if (expression->op() == AlgebraOp::Positive) return simplifyNode(expression->lhs());
	if (expression->op() == AlgebraOp::Negate) {
		auto operand = simplifyNode(expression->lhs());
		if (operand->op() == AlgebraOp::Number) return AlgebraExpression::number(-operand->value());
		if (operand->op() == AlgebraOp::Negate) return operand->lhs();
		return AlgebraExpression::unary(AlgebraOp::Negate, operand);
	}

	auto lhs = simplifyNode(expression->lhs());
	auto rhs = simplifyNode(expression->rhs());
	const auto op = expression->op();
	if (lhs->op() == AlgebraOp::Number && rhs->op() == AlgebraOp::Number) {
		double value = 0.0;
		switch (op) {
		case AlgebraOp::Add: value = lhs->value() + rhs->value(); break;
		case AlgebraOp::Subtract: value = lhs->value() - rhs->value(); break;
		case AlgebraOp::Multiply: value = lhs->value() * rhs->value(); break;
		case AlgebraOp::Divide:
			if (std::abs(rhs->value()) < epsilon) throw std::invalid_argument("Dijeljenje nulom");
			value = lhs->value() / rhs->value();
			break;
		case AlgebraOp::Power: value = std::pow(lhs->value(), rhs->value()); break;
		default: break;
		}
		if (!std::isfinite(value)) throw std::invalid_argument("Rezultat nije konacan broj");
		return AlgebraExpression::number(value);
	}
	if (op == AlgebraOp::Add && isNumber(lhs, 0.0)) return rhs;
	if ((op == AlgebraOp::Add || op == AlgebraOp::Subtract) && isNumber(rhs, 0.0)) return lhs;
	if (op == AlgebraOp::Multiply && (isNumber(lhs, 0.0) || isNumber(rhs, 0.0))) return AlgebraExpression::number(0.0);
	if (op == AlgebraOp::Multiply && isNumber(lhs, 1.0)) return rhs;
	if (op == AlgebraOp::Multiply && isNumber(rhs, 1.0)) return lhs;
	if (op == AlgebraOp::Divide && isNumber(rhs, 1.0)) return lhs;
	if (op == AlgebraOp::Power && isNumber(rhs, 1.0)) return lhs;
	if (op == AlgebraOp::Power && isNumber(rhs, 0.0)) return AlgebraExpression::number(1.0);
	return combineTerms(AlgebraExpression::binary(op, lhs, rhs));
}

} // namespace

SimplificationResult AlgebraSimplifier::simplify(const AlgebraExpression::Ptr& expression) {
	const auto before = AlgebraFormatter::format(expression);
	auto current = expression;
	std::vector<std::string> steps;
	for (int iteration = 0; iteration < 16; ++iteration) {
		auto next = simplifyNode(current);
		const auto currentText = AlgebraFormatter::format(current);
		const auto nextText = AlgebraFormatter::format(next);
		if (currentText == nextText) break;
		steps.push_back(currentText + " = " + nextText);
		current = std::move(next);
	}
	if (steps.empty()) steps.push_back(before + " je vec pojednostavljen izraz");
	return {current, std::move(steps)};
}

} // namespace aksiomat::algebra
