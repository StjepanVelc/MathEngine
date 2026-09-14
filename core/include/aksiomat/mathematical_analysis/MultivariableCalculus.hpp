#pragma once

#include <functional>
#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct PartialDerivativeResult {
	std::string description;
	double x;
	double y;
	double partialX;
	double partialY;
	std::vector<double> gradient;
	double gradientMagnitude;
	std::vector<std::string> steps;
};

struct DirectionalDerivativeResult {
	double partialX;
	double partialY;
	double directionX;
	double directionY;
	double directionalDerivative;
	std::vector<std::string> steps;
};

class MultivariableCalculus {
public:
	// Numericki racuna parcijalne derivacije funkcije dvije varijable f(x, y) u tocki (x, y)
	// centralnim diferencijama, te gradijent i njegovu normu.
	static PartialDerivativeResult partialDerivatives(const std::function<double(double, double)>& f,
		const std::string& description, double x, double y);

	// Racuna usmjerenu derivaciju u smjeru jedinicnog vektora (directionX, directionY) koristeci
	// vec izracunate parcijalne derivacije: D_u f = grad(f) . u.
	static DirectionalDerivativeResult directionalDerivative(const std::function<double(double, double)>& f,
		double x, double y, double directionX, double directionY);
};

} // namespace aksiomat::mathematical_analysis
