#pragma once

#include <optional>
#include <string>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/integral_calculus/PolynomialExpr.hpp"

// Interni pomocni sloj za neodredene integrale (nije dio javnog API-ja).
namespace aksiomat::integral_calculus::detail {

using differential_calculus::FnExpr;
using differential_calculus::FnFunc;
using differential_calculus::FnOp;
using Ptr = FnExpr::Ptr;

struct Attempt {
	Ptr antiderivative;
	std::vector<std::string> steps;
	std::string method;
};
using Outcome = std::optional<Attempt>;

constexpr int kMaxDepth = 16;

Ptr num(double value);
Ptr var(const std::string& name);
Ptr add(Ptr a, Ptr b);
Ptr sub(Ptr a, Ptr b);
Ptr mul(Ptr a, Ptr b);
Ptr div(Ptr a, Ptr b);
Ptr neg(Ptr a);
Ptr pow(Ptr base, Ptr exponent);
Ptr call(FnFunc func, Ptr argument);
Ptr lnAbs(Ptr argument);

// Broj kao "lijepi" razlomak p/q (q <= 12) ako je blizu, inace obican decimalni broj.
Ptr coefficient(double value);
Ptr scaleBy(double c, Ptr e);
Ptr divideByConstant(Ptr e, double d);

bool containsVar(const Ptr& e, const std::string& variable);
std::string show(const Ptr& e);
Ptr replaceSubexpression(const Ptr& e, const Ptr& target, const Ptr& replacement);
void collectSubexpressions(const Ptr& e, std::vector<Ptr>& out);

struct Linear {
	double a = 0.0;
	double b = 0.0;
};
// Prepoznaje izraz oblika a*x + b (a != 0).
std::optional<Linear> asLinear(const Ptr& e, const std::string& variable);

struct Factors {
	double coefficient = 1.0;
	std::vector<Ptr> numerators;
	std::vector<Ptr> denominators;
};
// Rastavlja umnozak/kvocijent na konstantu i faktore koji ovise o varijabli.
Factors flattenFactors(const Ptr& e, const std::string& variable);
Ptr rebuildProduct(const Factors& factors);

// Skracuje i spaja potencije istih baza u umnosku/kvocijentu (x^2 * 1/x -> x).
Ptr normalizeProduct(const Ptr& e, const std::string& variable);

// Zastita od eksplozije pretrazivanja: svaki integrate() poziv trosi jedinicu budzeta.
void resetBudget(int calls);
bool consumeBudget();

Ptr integratePolynomialExpr(const Coefficients& c, const std::string& variable);
Ptr differentiate(const Ptr& e);

Outcome integrate(const Ptr& f, const std::string& variable, int depth);
Outcome tableIntegral(const Ptr& f, const std::string& variable, int depth);
Outcome integrateBySubstitution(const Ptr& f, const std::string& variable, int depth);
Outcome integrateByParts(const Ptr& f, const std::string& variable, int depth);
Outcome integrateRational(const Ptr& f, const std::string& variable, int depth);

} // namespace aksiomat::integral_calculus::detail
