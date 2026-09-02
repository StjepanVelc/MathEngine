#pragma once

#include <string>
#include <vector>

namespace aksiomat::calculus_basics {

struct DefiniteIntegralResult {
	std::string expression;
	std::string antiderivative;
	double lowerBound;
	double upperBound;
	double area;
	double numericCheck;
	std::vector<std::string> steps;
};

class DefiniteIntegral {
public:
	// Racuna odredeni integral polinoma na [a,b] koristeci antiderivaciju (obrnuto pravilo potencije)
	// te vraca i neovisnu numericku (Simpsonovu) provjeru rezultata.
	static DefiniteIntegralResult evaluate(const std::string& expression, double lowerBound, double upperBound,
		const std::string& variable = "x");
};

} // namespace aksiomat::calculus_basics
