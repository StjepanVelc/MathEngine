#pragma once

#include <functional>
#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct ImproperIntegralResult {
	std::string description;
	double lowerBound;
	bool convergent;
	double approximateValue;
	std::vector<double> partialSums;
	std::vector<std::string> steps;
};

struct SubstitutionIntegralResult {
	double linearCoefficientA;
	double linearCoefficientB;
	int power;
	double lowerBound;
	double upperBound;
	double value;
	std::vector<std::string> steps;
};

class AdvancedIntegrals {
public:
	// Numericki procjenjuje neprivi integral integrand(x) dx od lowerBound do beskonacno,
	// promatrajuci konvergenciju parcijalnih suma na sve vecim gornjim granicama.
	static ImproperIntegralResult improperIntegral(const std::function<double(double)>& integrand,
		const std::string& description, double lowerBound);

	// Racuna odredjeni integral (ax+b)^power dx na [lowerBound, upperBound] metodom supstitucije u = ax+b.
	static SubstitutionIntegralResult integrateBySubstitution(double a, double b, int power, double lowerBound, double upperBound);
};

} // namespace aksiomat::mathematical_analysis
