#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct IntegralApplicationResult {
	std::string quantity;     // "volumen rotacije" ili "duljina luka"
	std::string expression;
	std::string integrand;    // podintegralna funkcija koja se integrira
	double lower = 0.0;
	double upper = 0.0;
	double value = 0.0;
	bool exact = false;       // true ako je pronadena antiderivacija
	std::string antiderivative;
	std::vector<std::string> steps;
};

class IntegralApplications {
public:
	// Volumen tijela nastalog rotacijom grafa f oko osi x: V = pi * ∫ f(x)^2 dx (metoda diskova).
	static IntegralApplicationResult volumeOfRevolution(const std::string& expression, double a, double b);

	// Duljina luka grafa f na [a, b]: L = ∫ sqrt(1 + f'(x)^2) dx.
	static IntegralApplicationResult arcLength(const std::string& expression, double a, double b);
};

} // namespace aksiomat::integral_calculus
