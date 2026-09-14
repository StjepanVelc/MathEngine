#include "aksiomat/mathematical_analysis/FunctionSeries.hpp"

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace aksiomat::mathematical_analysis {
namespace {

double factorial(int n) {
	double result = 1.0;
	for (int i = 2; i <= n; ++i) result *= i;
	return result;
}

} // namespace

TaylorSeriesResult FunctionSeries::buildTaylorSeries(const std::function<double(int, double)>& derivativeAt,
	const std::function<double(double)>& actualFunction, const std::string& description, double center,
	int order, double point) {
	if (order < 0) throw std::invalid_argument("Red Taylorovog reda ne smije biti negativan");
	if (!std::isfinite(center) || !std::isfinite(point)) throw std::invalid_argument("Tocke moraju biti konacni brojevi");

	TaylorSeriesResult result;
	result.description = description;
	result.center = center;
	result.order = order;
	result.point = point;

	double approximation = 0.0;
	for (int n = 0; n <= order; ++n) {
		const double coefficient = derivativeAt(n, center) / factorial(n);
		result.coefficients.push_back(coefficient);
		approximation += coefficient * std::pow(point - center, n);
	}

	result.taylorApproximationAtPoint = approximation;
	result.actualValueAtPoint = actualFunction(point);
	result.approximationError = std::abs(result.actualValueAtPoint - result.taylorApproximationAtPoint);

	std::ostringstream stepText;
	stepText << "Taylorov red funkcije " << description << " oko tocke a = " << center << " reda " << order << ".";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Opcenita formula: T(x) = suma_{n=0}^{" + std::to_string(order) + "} f^(n)(a)/n! * (x - a)^n.");
	result.steps.push_back("Racunamo koeficijente c_n = f^(n)(a)/n! za svaki n od 0 do " + std::to_string(order) + ".");
	result.steps.push_back("Aproksimacija u tocki x = " + std::to_string(point) + " iznosi T(x) = " +
		std::to_string(result.taylorApproximationAtPoint) + ", dok je stvarna vrijednost f(x) = " +
		std::to_string(result.actualValueAtPoint) + ".");
	result.steps.push_back("Pogreska aproksimacije |f(x) - T(x)| = " + std::to_string(result.approximationError) +
		"; veci red obicno smanjuje ovu pogresku blizu tocke a.");

	return result;
}

PowerSeriesConvergenceResult FunctionSeries::analyzePowerSeries(const std::vector<double>& coefficients, double testPoint) {
	if (coefficients.empty()) throw std::invalid_argument("Red potencija mora imati barem jedan koeficijent");

	PowerSeriesConvergenceResult result;
	result.coefficients = coefficients;
	result.testPoint = testPoint;

	// Omjer test: R = lim |a_n / a_{n+1}|, procijenjen iz zadnjih dostupnih koeficijenata.
	double radius = std::numeric_limits<double>::infinity();
	for (std::size_t i = 0; i + 1 < coefficients.size(); ++i) {
		if (coefficients[i + 1] != 0.0) {
			radius = std::abs(coefficients[i] / coefficients[i + 1]);
		}
	}
	result.radiusOfConvergence = radius;
	result.convergesAtPoint = std::isfinite(radius) ? std::abs(testPoint) < radius : true;

	result.steps.push_back("Koristimo omjer test na koeficijentima reda potencija: R = lim |a_n / a_(n+1)|.");
	result.steps.push_back("Procijenjeni radijus konvergencije iznosi R = " + std::to_string(result.radiusOfConvergence) + ".");
	result.steps.push_back(std::string("Testna tocka x = ") + std::to_string(testPoint) +
		(result.convergesAtPoint ? " lezi unutar radijusa konvergencije, pa red konvergira." :
			" lezi izvan radijusa konvergencije, pa red divergira."));

	return result;
}

} // namespace aksiomat::mathematical_analysis
