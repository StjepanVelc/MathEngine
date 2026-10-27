#pragma once

#include <string>
#include <vector>

#include "aksiomat/differential_calculus/RealFunctionLimits.hpp"

namespace aksiomat::differential_calculus {

enum class ContinuityKind {
	Continuous,
	Removable,
	Jump,
	Infinite,
	Oscillating,
	OutsideDomain
};

struct ContinuityAnalysis {
	std::string expression;
	double point = 0.0;
	bool valueDefined = false;
	double value = 0.0;
	OneSidedLimit left;
	OneSidedLimit right;
	ContinuityKind kind = ContinuityKind::Continuous;
	// "continuous", "removable", "jump", "infinite", "oscillating", "outside_domain"
	std::string kindId;
	std::vector<std::string> steps;
};

class FunctionContinuity {
public:
	// Neprekidnost u tocki: f(a) definiran, lijevi i desni limes postoje i jednaki su f(a).
	// Inace klasificira prekid: uklonjiv, skok, beskonacni, oscilirajuci.
	static ContinuityAnalysis analyze(const std::string& expression, double point, const std::string& variable = "x");
};

} // namespace aksiomat::differential_calculus
