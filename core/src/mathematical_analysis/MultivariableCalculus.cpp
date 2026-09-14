#include "aksiomat/mathematical_analysis/MultivariableCalculus.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::mathematical_analysis {
namespace {

constexpr double kStep = 1e-5;

} // namespace

PartialDerivativeResult MultivariableCalculus::partialDerivatives(const std::function<double(double, double)>& f,
	const std::string& description, double x, double y) {
	if (!std::isfinite(x) || !std::isfinite(y)) throw std::invalid_argument("Tocke moraju biti konacni brojevi");

	PartialDerivativeResult result;
	result.description = description;
	result.x = x;
	result.y = y;

	result.partialX = (f(x + kStep, y) - f(x - kStep, y)) / (2 * kStep);
	result.partialY = (f(x, y + kStep) - f(x, y - kStep)) / (2 * kStep);
	result.gradient = {result.partialX, result.partialY};
	result.gradientMagnitude = std::sqrt(result.partialX * result.partialX + result.partialY * result.partialY);

	std::ostringstream stepText;
	stepText << "Funkcija f(x, y) = " << description << " u tocki (" << x << ", " << y << ").";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Parcijalna derivacija po x: df/dx = " + std::to_string(result.partialX) +
		" (drzimo y konstantnim i derivirmo po x).");
	result.steps.push_back("Parcijalna derivacija po y: df/dy = " + std::to_string(result.partialY) +
		" (drzimo x konstantnim i derivirmo po y).");
	result.steps.push_back("Gradijent grad f = (df/dx, df/dy) pokazuje smjer najbrzeg rasta funkcije, s iznosom |grad f| = " +
		std::to_string(result.gradientMagnitude) + ".");

	return result;
}

DirectionalDerivativeResult MultivariableCalculus::directionalDerivative(const std::function<double(double, double)>& f,
	double x, double y, double directionX, double directionY) {
	const double norm = std::sqrt(directionX * directionX + directionY * directionY);
	if (norm < 1e-12) throw std::invalid_argument("Vektor smjera ne smije biti nul-vektor");

	const double unitX = directionX / norm;
	const double unitY = directionY / norm;

	DirectionalDerivativeResult result;
	result.partialX = (f(x + kStep, y) - f(x - kStep, y)) / (2 * kStep);
	result.partialY = (f(x, y + kStep) - f(x, y - kStep)) / (2 * kStep);
	result.directionX = unitX;
	result.directionY = unitY;
	result.directionalDerivative = result.partialX * unitX + result.partialY * unitY;

	result.steps.push_back("Normaliziramo vektor smjera na jedinicnu duljinu: u = (" + std::to_string(unitX) + ", " +
		std::to_string(unitY) + ").");
	result.steps.push_back("Racunamo gradijent u tocki: grad f = (" + std::to_string(result.partialX) + ", " +
		std::to_string(result.partialY) + ").");
	result.steps.push_back("Usmjerena derivacija: D_u f = grad f . u = " + std::to_string(result.directionalDerivative) + ".");

	return result;
}

} // namespace aksiomat::mathematical_analysis
