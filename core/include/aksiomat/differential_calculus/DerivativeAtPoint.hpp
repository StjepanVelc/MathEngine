#pragma once

#include <string>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::differential_calculus {

struct DifferenceQuotientRow {
	double h = 0.0;
	double x = 0.0;
	double secantSlope = 0.0;
	bool defined = false;
};

struct TangentResult {
	std::string expression;
	std::string derivativeExpression;
	double point = 0.0;
	double value = 0.0;
	bool valueDefined = false;
	double leftDerivative = 0.0;
	double rightDerivative = 0.0;
	bool differentiable = false;
	double slope = 0.0;
	// "", "kink", "vertical", "one_sided", "undefined"
	std::string problem;
	std::string tangentLine;
	std::string normalLine;
	std::vector<DifferenceQuotientRow> rightRows;
	std::vector<DifferenceQuotientRow> leftRows;
	std::vector<std::string> steps;
};

struct MeanValueResult {
	std::string expression;
	double a = 0.0;
	double b = 0.0;
	double fa = 0.0;
	double fb = 0.0;
	double meanSlope = 0.0;
	bool continuousOnInterval = false;
	bool differentiableInInterior = false;
	bool lagrangeApplies = false;
	bool rolleApplies = false;
	std::vector<double> points;
	std::vector<double> pointValues;
	std::vector<std::string> steps;
};

class DerivativeAtPoint {
public:
	// Derivacija po definiciji u tocki: kvocijent diferencija (f(a+h)-f(a))/h kad h -> 0 s obje strane,
	// jednostrane derivacije, simbolicka derivacija, jednadzba tangente i normale.
	static TangentResult analyze(const std::string& expression, double point, const std::string& variable = "x");

	// Rolleov i Lagrangeov teorem na [a, b]: provjera uvjeta i pronalazak tocaka c s f'(c) = (f(b)-f(a))/(b-a).
	static MeanValueResult meanValue(const std::string& expression, double a, double b,
		const std::string& variable = "x");
};

} // namespace aksiomat::differential_calculus
