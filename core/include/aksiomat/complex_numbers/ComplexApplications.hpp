#pragma once

#include <string>
#include <vector>

#include "aksiomat/complex_numbers/ComplexNumbers.hpp"

namespace aksiomat::complex_numbers {

struct PowerResult {
	ComplexValue result;
	std::vector<std::string> steps;
};

struct RootsResult {
	std::vector<ComplexValue> roots;
	std::vector<std::string> steps;
};

struct QuadraticEquationResult {
	ComplexValue root1;
	ComplexValue root2;
	std::vector<std::string> steps;
};

// Primjene kompleksnih brojeva: potenciranje (De Moivre), n-ti korijeni
// i rjesavanje kvadratne jednadzbe s kompleksnim rjesenjima.
class ComplexApplications {
public:
	static PowerResult power(ComplexValue base, int exponent);
	static RootsResult nthRoots(ComplexValue value, int degree);
	static QuadraticEquationResult solveQuadraticEquation(double a, double b, double c);
};

} // namespace aksiomat::complex_numbers
