#include "aksiomat/mathematical_analysis/DifferentialEquations.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::mathematical_analysis {

namespace {

constexpr double maximumOdeSteps = 5000.0;

void requireBoundedStepCount(double initialT, double finalT, double stepSize) {
	const double span = finalT - initialT;
	const double estimatedSteps = std::ceil(span / stepSize);
	if (!std::isfinite(estimatedSteps) || estimatedSteps > maximumOdeSteps) {
		throw std::invalid_argument("Prevelik broj koraka (vise od " + std::to_string(static_cast<long long>(maximumOdeSteps)) + "). Povecaj velicinu koraka ili smanji interval.");
	}
}

} // namespace

OdeSolutionResult DifferentialEquations::solveEuler(const std::function<double(double, double)>& f,
	const std::string& description, double initialT, double initialY, double finalT, double stepSize) {
	if (stepSize <= 0.0) throw std::invalid_argument("Korak mora biti pozitivan");
	if (finalT < initialT) throw std::invalid_argument("Konacni t mora biti veci ili jednak pocetnom t");
	requireBoundedStepCount(initialT, finalT, stepSize);

	OdeSolutionResult result;
	result.description = description;
	result.method = "Euler";
	result.initialT = initialT;
	result.initialY = initialY;
	result.stepSize = stepSize;

	double t = initialT;
	double y = initialY;
	result.points.push_back({t, y});
	while (t < finalT - 1e-12) {
		const double h = std::min(stepSize, finalT - t);
		y = y + h * f(t, y);
		t = t + h;
		result.points.push_back({t, y});
	}
	result.finalValue = y;

	std::ostringstream stepText;
	stepText << "Rjesavamo y' = " << description << ", y(" << initialT << ") = " << initialY << " Eulerovom metodom.";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Iterativna formula: y_(n+1) = y_n + h * f(t_n, y_n), s korakom h = " + std::to_string(stepSize) + ".");
	result.steps.push_back("Nakon " + std::to_string(result.points.size() - 1) + " koraka dolazimo do t = " +
		std::to_string(t) + ", y = " + std::to_string(result.finalValue) + ".");

	return result;
}

OdeSolutionResult DifferentialEquations::solveRungeKutta4(const std::function<double(double, double)>& f,
	const std::string& description, double initialT, double initialY, double finalT, double stepSize) {
	if (stepSize <= 0.0) throw std::invalid_argument("Korak mora biti pozitivan");
	if (finalT < initialT) throw std::invalid_argument("Konacni t mora biti veci ili jednak pocetnom t");
	requireBoundedStepCount(initialT, finalT, stepSize);

	OdeSolutionResult result;
	result.description = description;
	result.method = "Runge-Kutta 4";
	result.initialT = initialT;
	result.initialY = initialY;
	result.stepSize = stepSize;

	double t = initialT;
	double y = initialY;
	result.points.push_back({t, y});
	while (t < finalT - 1e-12) {
		const double h = std::min(stepSize, finalT - t);
		const double k1 = f(t, y);
		const double k2 = f(t + h / 2, y + h / 2 * k1);
		const double k3 = f(t + h / 2, y + h / 2 * k2);
		const double k4 = f(t + h, y + h * k3);
		y = y + (h / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4);
		t = t + h;
		result.points.push_back({t, y});
	}
	result.finalValue = y;

	std::ostringstream stepText;
	stepText << "Rjesavamo y' = " << description << ", y(" << initialT << ") = " << initialY << " metodom Runge-Kutta 4. reda.";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Svaki korak koristi cetiri procjene nagiba k1..k4 kombinirane u tezinski prosjek, sto daje vecu tocnost od Eulerove metode.");
	result.steps.push_back("Nakon " + std::to_string(result.points.size() - 1) + " koraka dolazimo do t = " +
		std::to_string(t) + ", y = " + std::to_string(result.finalValue) + ".");

	return result;
}

} // namespace aksiomat::mathematical_analysis
