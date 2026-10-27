#include "aksiomat/differential_calculus/FunctionExpression.hpp"

#include <cmath>
#include <limits>
#include <numbers>
#include <sstream>
#include <stdexcept>

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;

constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

double applyFunction(FnFunc func, double v) {
	switch (func) {
	case FnFunc::Sin: return std::sin(v);
	case FnFunc::Cos: return std::cos(v);
	case FnFunc::Tan: return std::tan(v);
	case FnFunc::Exp: return std::exp(v);
	case FnFunc::Ln: return v > 0.0 ? std::log(v) : (v == 0.0 ? -std::numeric_limits<double>::infinity() : kNaN);
	case FnFunc::Sqrt: return v >= 0.0 ? std::sqrt(v) : kNaN;
	case FnFunc::Abs: return std::fabs(v);
	case FnFunc::Asin: return (v >= -1.0 && v <= 1.0) ? std::asin(v) : kNaN;
	case FnFunc::Acos: return (v >= -1.0 && v <= 1.0) ? std::acos(v) : kNaN;
	case FnFunc::Atan: return std::atan(v);
	case FnFunc::Sinh: return std::sinh(v);
	case FnFunc::Cosh: return std::cosh(v);
	case FnFunc::Tanh: return std::tanh(v);
	}
	return kNaN;
}

bool isNumber(const Ptr& e) { return e->op() == FnOp::Number; }
bool isNumber(const Ptr& e, double v) { return isNumber(e) && e->value() == v; }

bool isNiceNumber(double v) {
	return std::isfinite(v) && std::fabs(v) < 1e15 && std::fabs(v - std::round(v)) < 1e-12;
}

std::string formatNumber(double v) {
	if (v == std::numbers::e) return "e";
	if (v == std::numbers::pi) return "pi";
	if (isNiceNumber(v)) {
		std::ostringstream out;
		out << static_cast<long long>(std::llround(v));
		return out.str();
	}
	std::ostringstream out;
	out.precision(10);
	out << v;
	return out.str();
}

int precedence(const Ptr& e) {
	switch (e->op()) {
	case FnOp::Add:
	case FnOp::Subtract: return 1;
	case FnOp::Multiply:
	case FnOp::Divide:
	case FnOp::Negate: return 2;
	case FnOp::Power: return 3;
	case FnOp::Number: return e->value() < 0.0 ? 2 : 4;
	default: return 4;
	}
}

bool isNegativeLike(const Ptr& e) {
	return e->op() == FnOp::Negate || (e->op() == FnOp::Number && e->value() < 0.0);
}

std::string str(const Ptr& e);

std::string wrap(const Ptr& e, bool parenthesize) {
	return parenthesize ? "(" + str(e) + ")" : str(e);
}

std::string str(const Ptr& e) {
	switch (e->op()) {
	case FnOp::Number: return formatNumber(e->value());
	case FnOp::Variable: return e->name();
	case FnOp::Negate: return "-" + wrap(e->lhs(), precedence(e->lhs()) < 2 || isNegativeLike(e->lhs()));
	case FnOp::Add: return str(e->lhs()) + " + " + str(e->rhs());
	case FnOp::Subtract: return str(e->lhs()) + " - " + wrap(e->rhs(), precedence(e->rhs()) <= 1);
	case FnOp::Multiply:
		return wrap(e->lhs(), precedence(e->lhs()) < 2) + "*" +
			wrap(e->rhs(), precedence(e->rhs()) < 2 || isNegativeLike(e->rhs()));
	case FnOp::Divide:
		return wrap(e->lhs(), precedence(e->lhs()) < 2) + "/" + wrap(e->rhs(), precedence(e->rhs()) <= 2);
	case FnOp::Power:
		return wrap(e->lhs(), precedence(e->lhs()) <= 3) + "^" + wrap(e->rhs(), precedence(e->rhs()) <= 3);
	case FnOp::Call: return std::string(functionName(e->func())) + "(" + str(e->lhs()) + ")";
	}
	return {};
}

Ptr simplifyOnce(const Ptr& e) {
	switch (e->op()) {
	case FnOp::Number:
	case FnOp::Variable: return e;
	case FnOp::Negate: {
		const Ptr a = simplifyOnce(e->lhs());
		if (isNumber(a)) return FnExpr::number(a->value() == 0.0 ? 0.0 : -a->value());
		if (a->op() == FnOp::Negate) return a->lhs();
		return FnExpr::unary(FnOp::Negate, a);
	}
	case FnOp::Call: {
		const Ptr a = simplifyOnce(e->lhs());
		if (isNumber(a)) {
			const double r = applyFunction(e->func(), a->value());
			if (isNiceNumber(r)) return FnExpr::number(r == 0.0 ? 0.0 : r);
		}
		return FnExpr::call(e->func(), a);
	}
	default: break;
	}

	const Ptr a = simplifyOnce(e->lhs());
	const Ptr b = simplifyOnce(e->rhs());
	switch (e->op()) {
	case FnOp::Add:
		if (isNumber(a) && isNumber(b)) return FnExpr::number(a->value() + b->value());
		if (isNumber(a, 0.0)) return b;
		if (isNumber(b, 0.0)) return a;
		if (b->op() == FnOp::Negate) return FnExpr::binary(FnOp::Subtract, a, b->lhs());
		if (isNumber(b) && b->value() < 0.0) return FnExpr::binary(FnOp::Subtract, a, FnExpr::number(-b->value()));
		if (str(a) == str(b)) return FnExpr::binary(FnOp::Multiply, FnExpr::number(2.0), a);
		return FnExpr::binary(FnOp::Add, a, b);
	case FnOp::Subtract:
		if (isNumber(a) && isNumber(b)) return FnExpr::number(a->value() - b->value());
		if (isNumber(b, 0.0)) return a;
		if (isNumber(a, 0.0)) return FnExpr::unary(FnOp::Negate, b);
		if (str(a) == str(b)) return FnExpr::number(0.0);
		if (b->op() == FnOp::Negate) return FnExpr::binary(FnOp::Add, a, b->lhs());
		return FnExpr::binary(FnOp::Subtract, a, b);
	case FnOp::Multiply:
		if (isNumber(a) && isNumber(b)) return FnExpr::number(a->value() * b->value());
		if (isNumber(a, 0.0) || isNumber(b, 0.0)) return FnExpr::number(0.0);
		if (isNumber(a, 1.0)) return b;
		if (isNumber(b, 1.0)) return a;
		if (isNumber(a, -1.0)) return FnExpr::unary(FnOp::Negate, b);
		if (isNumber(b, -1.0)) return FnExpr::unary(FnOp::Negate, a);
		if (isNumber(b)) return FnExpr::binary(FnOp::Multiply, b, a);
		if (a->op() == FnOp::Negate && b->op() == FnOp::Negate) return FnExpr::binary(FnOp::Multiply, a->lhs(), b->lhs());
		if (a->op() == FnOp::Negate) return FnExpr::unary(FnOp::Negate, FnExpr::binary(FnOp::Multiply, a->lhs(), b));
		if (b->op() == FnOp::Negate) return FnExpr::unary(FnOp::Negate, FnExpr::binary(FnOp::Multiply, a, b->lhs()));
		if (isNumber(a) && b->op() == FnOp::Multiply && isNumber(b->lhs())) {
			return FnExpr::binary(FnOp::Multiply, FnExpr::number(a->value() * b->lhs()->value()), b->rhs());
		}
		if (str(a) == str(b)) return FnExpr::binary(FnOp::Power, a, FnExpr::number(2.0));
		return FnExpr::binary(FnOp::Multiply, a, b);
	case FnOp::Divide:
		if (isNumber(a) && isNumber(b) && b->value() != 0.0) {
			const double r = a->value() / b->value();
			if (isNiceNumber(r)) return FnExpr::number(r);
		}
		if (isNumber(a, 0.0) && !isNumber(b, 0.0)) return FnExpr::number(0.0);
		if (isNumber(b, 1.0)) return a;
		if (isNumber(b, -1.0)) return FnExpr::unary(FnOp::Negate, a);
		return FnExpr::binary(FnOp::Divide, a, b);
	case FnOp::Power:
		if (isNumber(a) && isNumber(b)) {
			const double r = std::pow(a->value(), b->value());
			if (isNiceNumber(r)) return FnExpr::number(r);
		}
		if (isNumber(b, 1.0)) return a;
		if (isNumber(b, 0.0)) return FnExpr::number(1.0);
		if (isNumber(a, 1.0)) return FnExpr::number(1.0);
		return FnExpr::binary(FnOp::Power, a, b);
	default: break;
	}
	return e;
}

} // namespace

FnExpr::FnExpr(FnOp op, FnFunc func, double value, std::string name, Ptr lhs, Ptr rhs)
	: op_(op), func_(func), value_(value), name_(std::move(name)), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

FnExpr::Ptr FnExpr::number(double value) {
	return Ptr(new FnExpr(FnOp::Number, FnFunc::Sin, value, {}, nullptr, nullptr));
}

FnExpr::Ptr FnExpr::variable(std::string name) {
	return Ptr(new FnExpr(FnOp::Variable, FnFunc::Sin, 0.0, std::move(name), nullptr, nullptr));
}

FnExpr::Ptr FnExpr::unary(FnOp op, Ptr operand) {
	return Ptr(new FnExpr(op, FnFunc::Sin, 0.0, {}, std::move(operand), nullptr));
}

FnExpr::Ptr FnExpr::binary(FnOp op, Ptr lhs, Ptr rhs) {
	return Ptr(new FnExpr(op, FnFunc::Sin, 0.0, {}, std::move(lhs), std::move(rhs)));
}

FnExpr::Ptr FnExpr::call(FnFunc func, Ptr argument) {
	return Ptr(new FnExpr(FnOp::Call, func, 0.0, {}, std::move(argument), nullptr));
}

const char* functionName(FnFunc func) {
	switch (func) {
	case FnFunc::Sin: return "sin";
	case FnFunc::Cos: return "cos";
	case FnFunc::Tan: return "tan";
	case FnFunc::Exp: return "exp";
	case FnFunc::Ln: return "ln";
	case FnFunc::Sqrt: return "sqrt";
	case FnFunc::Abs: return "abs";
	case FnFunc::Asin: return "asin";
	case FnFunc::Acos: return "acos";
	case FnFunc::Atan: return "atan";
	case FnFunc::Sinh: return "sinh";
	case FnFunc::Cosh: return "cosh";
	case FnFunc::Tanh: return "tanh";
	}
	return "";
}

double evaluate(const FnExpr::Ptr& e, double x) {
	if (!e) throw std::invalid_argument("Prazan izraz");
	switch (e->op()) {
	case FnOp::Number: return e->value();
	case FnOp::Variable: return x;
	case FnOp::Negate: return -evaluate(e->lhs(), x);
	case FnOp::Add: return evaluate(e->lhs(), x) + evaluate(e->rhs(), x);
	case FnOp::Subtract: return evaluate(e->lhs(), x) - evaluate(e->rhs(), x);
	case FnOp::Multiply: return evaluate(e->lhs(), x) * evaluate(e->rhs(), x);
	case FnOp::Divide: return evaluate(e->lhs(), x) / evaluate(e->rhs(), x);
	case FnOp::Power: return std::pow(evaluate(e->lhs(), x), evaluate(e->rhs(), x));
	case FnOp::Call: return applyFunction(e->func(), evaluate(e->lhs(), x));
	}
	return kNaN;
}

std::string toString(const FnExpr::Ptr& e) {
	if (!e) throw std::invalid_argument("Prazan izraz");
	return str(e);
}

FnExpr::Ptr simplify(const FnExpr::Ptr& e) {
	if (!e) throw std::invalid_argument("Prazan izraz");
	Ptr current = e;
	std::string text = str(current);
	for (int pass = 0; pass < 12; ++pass) {
		Ptr next = simplifyOnce(current);
		std::string nextText = str(next);
		current = next;
		if (nextText == text) break;
		text = std::move(nextText);
	}
	return current;
}

bool dependsOnVariable(const FnExpr::Ptr& e) {
	if (!e) return false;
	if (e->op() == FnOp::Variable) return true;
	if (e->op() == FnOp::Number) return false;
	return dependsOnVariable(e->lhs()) || dependsOnVariable(e->rhs());
}

} // namespace aksiomat::differential_calculus
