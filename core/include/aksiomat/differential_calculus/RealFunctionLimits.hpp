#pragma once

#include <string>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::differential_calculus {

enum class LimitKind {
	Finite,
	PositiveInfinity,
	NegativeInfinity,
	DoesNotExist,
	Undefined
};

struct OneSidedLimit {
	LimitKind kind = LimitKind::Undefined;
	double value = 0.0;
};

struct LimitSample {
	double x = 0.0;
	double value = 0.0;
	bool defined = false;
};

struct RealLimitResult {
	std::string expression;
	double point = 0.0;
	bool atInfinity = false;
	bool valueDefinedAtPoint = false;
	double valueAtPoint = 0.0;
	OneSidedLimit left;
	OneSidedLimit right;
	OneSidedLimit combined;
	std::string indeterminateForm;
	bool lHopitalUsed = false;
	std::vector<LimitSample> leftSamples;
	std::vector<LimitSample> rightSamples;
	std::vector<std::string> steps;
};

class RealFunctionLimits {
public:
	// Limes funkcije kad x -> point (point moze biti +-beskonacno, tj. +-std::numeric_limits<double>::infinity()).
	// Numericki jednostrani limesi + prepoznavanje neodredjenih oblika (0/0, inf/inf, 0*inf, inf-inf,
	// 1^inf, 0^0, inf^0) i L'Hopitalovo pravilo korak po korak.
	static RealLimitResult evaluate(const std::string& expression, double point, const std::string& variable = "x");

	// Jednostrani numericki limes (direction = -1 slijeva, +1 zdesna; za point = +-inf se ignorira).
	static OneSidedLimit oneSided(const FnExpr::Ptr& expression, double point, int direction);

	// Tekstualni opis limesa ("3", "+inf", "ne postoji", ...).
	static std::string describe(const OneSidedLimit& limit);
};

} // namespace aksiomat::differential_calculus
