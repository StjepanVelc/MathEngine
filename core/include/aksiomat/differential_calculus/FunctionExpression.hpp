#pragma once

#include <memory>
#include <string>

namespace aksiomat::differential_calculus {

enum class FnOp {
	Number,
	Variable,
	Add,
	Subtract,
	Multiply,
	Divide,
	Power,
	Negate,
	Call
};

enum class FnFunc {
	Sin,
	Cos,
	Tan,
	Exp,
	Ln,
	Sqrt,
	Abs,
	Asin,
	Acos,
	Atan,
	Sinh,
	Cosh,
	Tanh
};

// Nepromjenjivo stablo izraza realne funkcije jedne varijable (sin, cos, exp, ln, ...).
class FnExpr {
public:
	using Ptr = std::shared_ptr<const FnExpr>;

	static Ptr number(double value);
	static Ptr variable(std::string name);
	static Ptr unary(FnOp op, Ptr operand);
	static Ptr binary(FnOp op, Ptr lhs, Ptr rhs);
	static Ptr call(FnFunc func, Ptr argument);

	FnOp op() const { return op_; }
	FnFunc func() const { return func_; }
	double value() const { return value_; }
	const std::string& name() const { return name_; }
	const Ptr& lhs() const { return lhs_; }
	const Ptr& rhs() const { return rhs_; }

private:
	FnExpr(FnOp op, FnFunc func, double value, std::string name, Ptr lhs, Ptr rhs);

	FnOp op_;
	FnFunc func_ = FnFunc::Sin;
	double value_ = 0.0;
	std::string name_;
	Ptr lhs_;
	Ptr rhs_;
};

// Parser: podrzava + - * / ^, implicitno mnozenje (2x, 3sin(x)), pi, e i funkcije
// sin, cos, tan, exp, ln, log (= ln), sqrt, abs, asin, acos, atan, sinh, cosh, tanh.
// Baca std::invalid_argument za neispravan unos.
FnExpr::Ptr parseFunction(const std::string& text, const std::string& variable = "x");

// Vrijednost izraza u tocki; NaN ako funkcija u toj tocki nije definirana.
double evaluate(const FnExpr::Ptr& expression, double x);

// Citljiv zapis izraza (npr. "2*x^3 + sin(x)").
std::string toString(const FnExpr::Ptr& expression);

// Algebarsko pojednostavljenje (0+a, 1*a, a^1, konstantni izrazi, ...).
FnExpr::Ptr simplify(const FnExpr::Ptr& expression);

// Ovisi li izraz o varijabli.
bool dependsOnVariable(const FnExpr::Ptr& expression);

// Naziv funkcije kako se zapisuje u izrazu.
const char* functionName(FnFunc func);

} // namespace aksiomat::differential_calculus
