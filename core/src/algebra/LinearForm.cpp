#include "aksiomat/algebra/LinearForm.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

LinearForm add(const LinearForm& lhs, const LinearForm& rhs, double rhsSign = 1.0) {
	return {lhs.coefficient + rhsSign * rhs.coefficient, lhs.constant + rhsSign * rhs.constant};
}

std::string number(double value) {
	if (std::abs(value) < epsilon) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

} // namespace

LinearForm extractLinearForm(const AlgebraExpression::Ptr& expression, const std::string& variable) {
	if (!expression) throw std::invalid_argument("Nedostaje izraz");
	switch (expression->op()) {
	case AlgebraOp::Number: return {0.0, expression->value()};
	case AlgebraOp::Variable:
		if (expression->name() != variable) throw std::invalid_argument("Ocekivana je samo varijabla " + variable);
		return {1.0, 0.0};
	case AlgebraOp::Positive: return extractLinearForm(expression->lhs(), variable);
	case AlgebraOp::Negate: {
		const auto value = extractLinearForm(expression->lhs(), variable);
		return {-value.coefficient, -value.constant};
	}
	case AlgebraOp::Add: return add(extractLinearForm(expression->lhs(), variable), extractLinearForm(expression->rhs(), variable));
	case AlgebraOp::Subtract: return add(extractLinearForm(expression->lhs(), variable), extractLinearForm(expression->rhs(), variable), -1.0);
	case AlgebraOp::Multiply: {
		const auto lhs = extractLinearForm(expression->lhs(), variable);
		const auto rhs = extractLinearForm(expression->rhs(), variable);
		if (std::abs(lhs.coefficient) > epsilon && std::abs(rhs.coefficient) > epsilon) {
			throw std::invalid_argument("Izraz nije linearan");
		}
		if (std::abs(lhs.coefficient) > epsilon) return {lhs.coefficient * rhs.constant, lhs.constant * rhs.constant};
		if (std::abs(rhs.coefficient) > epsilon) return {rhs.coefficient * lhs.constant, rhs.constant * lhs.constant};
		return {0.0, lhs.constant * rhs.constant};
	}
	case AlgebraOp::Divide: {
		const auto lhs = extractLinearForm(expression->lhs(), variable);
		const auto rhs = extractLinearForm(expression->rhs(), variable);
		if (std::abs(rhs.coefficient) > epsilon) throw std::invalid_argument("Djelitelj ne smije sadrzavati varijablu");
		if (std::abs(rhs.constant) < epsilon) throw std::invalid_argument("Dijeljenje nulom");
		return {lhs.coefficient / rhs.constant, lhs.constant / rhs.constant};
	}
	case AlgebraOp::Power: {
		const auto base = extractLinearForm(expression->lhs(), variable);
		if (expression->rhs()->op() != AlgebraOp::Number) throw std::invalid_argument("Izraz nije linearan");
		const double exponent = expression->rhs()->value();
		if (std::abs(exponent - 1.0) < epsilon) return base;
		if (std::abs(exponent) < epsilon) return {0.0, 1.0};
		if (std::abs(base.coefficient) > epsilon) throw std::invalid_argument("Izraz nije linearan");
		return {0.0, std::pow(base.constant, exponent)};
	}
	}
	throw std::invalid_argument("Nepodrzan izraz");
}

std::string formatLinearForm(const LinearForm& form, const std::string& variable) {
	std::string result;
	if (std::abs(form.coefficient) >= epsilon) {
		if (std::abs(form.coefficient - 1.0) < epsilon) result = variable;
		else if (std::abs(form.coefficient + 1.0) < epsilon) result = "-" + variable;
		else result = number(form.coefficient) + variable;
	}
	if (std::abs(form.constant) >= epsilon || result.empty()) {
		if (!result.empty() && form.constant > 0.0) result += " + ";
		else if (!result.empty()) result += " - ";
		result += number(result.empty() ? form.constant : std::abs(form.constant));
	}
	return result;
}

} // namespace aksiomat::algebra
