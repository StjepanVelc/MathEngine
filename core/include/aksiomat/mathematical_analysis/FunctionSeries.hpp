#pragma once

#include <functional>
#include <string>
#include <vector>

namespace aksiomat::mathematical_analysis {

struct TaylorSeriesResult {
	std::string description;
	double center;
	int order;
	std::vector<double> coefficients;
	double point;
	double taylorApproximationAtPoint;
	double actualValueAtPoint;
	double approximationError;
	std::vector<std::string> steps;
};

struct PowerSeriesConvergenceResult {
	std::vector<double> coefficients;
	double radiusOfConvergence;
	bool convergesAtPoint;
	double testPoint;
	std::vector<std::string> steps;
};

class FunctionSeries {
public:
	// Gradi Taylorov red reda `order` oko tocke `center` za funkciju zadanu preko svojih derivacija
	// (derivativeAt(n, x) vraca n-tu derivaciju u tocki x), te ga usporedjuje sa stvarnom vrijednoscu u `point`.
	static TaylorSeriesResult buildTaylorSeries(const std::function<double(int, double)>& derivativeAt,
		const std::function<double(double)>& actualFunction, const std::string& description, double center,
		int order, double point);

	// Racuna radijus konvergencije reda potencija sa zadanim koeficijentima koristeci omjer susjednih koeficijenata,
	// te provjerava konvergira li red u testPoint (numerickom procjenom parcijalnih suma).
	static PowerSeriesConvergenceResult analyzePowerSeries(const std::vector<double>& coefficients, double testPoint);
};

} // namespace aksiomat::mathematical_analysis
