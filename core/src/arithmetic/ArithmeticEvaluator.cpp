#include "aksiomat/ArithmeticEvaluator.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat {
namespace {

void requireArity(const ArithmeticExpression::Ptr& expression, std::size_t expected) {
	if (expression->arguments().size() != expected) {
		throw std::invalid_argument("Funkcija '" + expression->name() + "' ocekuje " +
									std::to_string(expected) + " argument(a)");
	}
}

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Rezultat nije konacan broj");
}

} // namespace

double ArithmeticEvaluator::evaluate(const ArithmeticExpression::Ptr& expression) {
	const auto& arguments = expression->arguments();
	auto unary = [&] { return evaluate(arguments.at(0)); };
	auto lhs = [&] { return evaluate(arguments.at(0)); };
	auto rhs = [&] { return evaluate(arguments.at(1)); };

	double result = 0.0;
	switch (expression->op()) {
	case ArithmeticOp::Number:
		return expression->value();
	case ArithmeticOp::Constant:
		if (expression->name() == "pi") return std::numbers::pi;
		if (expression->name() == "e") return std::numbers::e;
		throw std::invalid_argument("Nepoznata konstanta '" + expression->name() + "'");
	case ArithmeticOp::Add:
		result = lhs() + rhs();
		break;
	case ArithmeticOp::Subtract:
		result = lhs() - rhs();
		break;
	case ArithmeticOp::Multiply:
		result = lhs() * rhs();
		break;
	case ArithmeticOp::Divide: {
		const double divisor = rhs();
		if (divisor == 0.0) throw std::invalid_argument("Dijeljenje s nulom");
		result = lhs() / divisor;
		break;
	}
	case ArithmeticOp::Modulo: {
		const double divisor = rhs();
		if (divisor == 0.0) throw std::invalid_argument("Modulo s nulom");
		result = std::fmod(lhs(), divisor);
		break;
	}
	case ArithmeticOp::Power:
		result = std::pow(lhs(), rhs());
		break;
	case ArithmeticOp::Negate:
		result = -unary();
		break;
	case ArithmeticOp::Positive:
		result = unary();
		break;
	case ArithmeticOp::Factorial: {
		const double value = unary();
		if (value < 0.0 || std::floor(value) != value) {
			throw std::invalid_argument("Faktorijel zahtijeva nenegativan cijeli broj");
		}
		if (value > 170.0) throw std::invalid_argument("Faktorijel je prevelik");
		result = 1.0;
		for (unsigned i = 2; i <= static_cast<unsigned>(value); ++i) result *= i;
		break;
	}
	case ArithmeticOp::Function: {
		const auto& name = expression->name();
		if (name == "sqrt") {
			requireArity(expression, 1);
			const double value = unary();
			if (value < 0.0) throw std::invalid_argument("Korijen negativnog broja nije realan");
			result = std::sqrt(value);
		} else if (name == "abs") {
			requireArity(expression, 1);
			result = std::abs(unary());
		} else if (name == "round") {
			requireArity(expression, 1);
			result = std::round(unary());
		} else if (name == "floor") {
			requireArity(expression, 1);
			result = std::floor(unary());
		} else if (name == "ceil") {
			requireArity(expression, 1);
			result = std::ceil(unary());
		} else if (name == "min" || name == "max" || name == "mod") {
			requireArity(expression, 2);
			const double left = lhs();
			const double right = rhs();
			if (name == "min") result = std::min(left, right);
			else if (name == "max") result = std::max(left, right);
			else {
				if (right == 0.0) throw std::invalid_argument("Modulo s nulom");
				result = std::fmod(left, right);
			}
		} else {
			throw std::invalid_argument("Nepoznata funkcija '" + name + "'");
		}
		break;
	}
	}
	requireFinite(result);
	return result;
}

} // namespace aksiomat
