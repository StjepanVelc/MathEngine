#pragma once

#include <functional>
#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct OdeSolutionPoint {
	double t;
	double y;
};

struct OdeSolutionResult {
	std::string description;
	std::string method;
	double initialT;
	double initialY;
	double stepSize;
	std::vector<OdeSolutionPoint> points;
	double finalValue;
	std::vector<std::string> steps;
};

class DifferentialEquations {
public:
	// Rjesava pocetni problem y' = f(t, y), y(initialT) = initialY, Eulerovom metodom do finalT.
	static OdeSolutionResult solveEuler(const std::function<double(double, double)>& f, const std::string& description,
		double initialT, double initialY, double finalT, double stepSize);

	// Rjesava isti problem preciznijom metodom Runge-Kutta 4. reda.
	static OdeSolutionResult solveRungeKutta4(const std::function<double(double, double)>& f, const std::string& description,
		double initialT, double initialY, double finalT, double stepSize);
};

} // namespace aksiomat::mathematical_analysis
