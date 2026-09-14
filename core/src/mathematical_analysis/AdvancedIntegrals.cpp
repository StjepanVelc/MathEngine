#include "aksiomat/mathematical_analysis/AdvancedIntegrals.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::mathematical_analysis {
namespace {

// Simpsonovo pravilo za numericku integraciju na [a, b] s n (parnim) podintervala.
double simpson(const std::function<double(double)>& f, double a, double b, int n) {
	if (n % 2 != 0) ++n;
	const double h = (b - a) / n;
	double sum = f(a) + f(b);
	for (int i = 1; i < n; ++i) {
		const double x = a + i * h;
		sum += (i % 2 == 0 ? 2.0 : 4.0) * f(x);
	}
	return sum * h / 3.0;
}

} // namespace

ImproperIntegralResult AdvancedIntegrals::improperIntegral(const std::function<double(double)>& integrand,
	const std::string& description, double lowerBound) {
	if (!std::isfinite(lowerBound)) throw std::invalid_argument("Donja granica mora biti konacan broj");

	ImproperIntegralResult result;
	result.description = description;
	result.lowerBound = lowerBound;

	const std::vector<double> upperBounds = {lowerBound + 10, lowerBound + 100, lowerBound + 1000, lowerBound + 10000};
	constexpr double targetStep = 0.005;
	constexpr int maxSamples = 200000;
	for (const double upper : upperBounds) {
		const int samples = std::min(maxSamples, std::max(200, static_cast<int>((upper - lowerBound) / targetStep)));
		result.partialSums.push_back(simpson(integrand, lowerBound, upper, samples));
	}

	const double last = result.partialSums.back();
	const double secondLast = result.partialSums[result.partialSums.size() - 2];
	result.convergent = std::isfinite(last) && std::abs(last - secondLast) < 1e-3;
	result.approximateValue = last;

	std::ostringstream stepText;
	stepText << "Promatramo integral " << description << " na [" << lowerBound << ", beskonacno).";
	result.steps.push_back(stepText.str());
	result.steps.push_back("Racunamo parcijalne integrale na sve vecim gornjim granicama i pratimo priblizavaju li se vrijednosti konstanti.");
	result.steps.push_back(std::string("Niz parcijalnih vrijednosti: ") +
		(result.convergent ? "stabilizira se, sto ukazuje na konvergenciju." : "ne stabilizira se, sto ukazuje na divergenciju."));
	if (result.convergent) {
		result.steps.push_back("Zakljucak: nepravi integral konvergira priblizno prema " + std::to_string(result.approximateValue) + ".");
	} else {
		result.steps.push_back("Zakljucak: nepravi integral divergira ili raste bez granice.");
	}

	return result;
}

SubstitutionIntegralResult AdvancedIntegrals::integrateBySubstitution(double a, double b, int power, double lowerBound, double upperBound) {
	if (a == 0.0) throw std::invalid_argument("Koeficijent a ne smije biti 0");
	if (power == -1) throw std::invalid_argument("Za power = -1 potreban je logaritamski slucaj, nije podrzano ovdje");

	SubstitutionIntegralResult result;
	result.linearCoefficientA = a;
	result.linearCoefficientB = b;
	result.power = power;
	result.lowerBound = lowerBound;
	result.upperBound = upperBound;

	// Integral (ax+b)^power dx = (ax+b)^(power+1) / (a*(power+1)) + C
	const auto antiderivative = [&](double x) {
		return std::pow(a * x + b, power + 1) / (a * (power + 1));
	};
	result.value = antiderivative(upperBound) - antiderivative(lowerBound);

	result.steps.push_back("Supstitucija: u = " + std::to_string(a) + "x + " + std::to_string(b) + ", du = " +
		std::to_string(a) + " dx.");
	result.steps.push_back("Integral postaje (1/" + std::to_string(a) + ") * integral u^" + std::to_string(power) + " du.");
	result.steps.push_back("Antiderivacija: u^" + std::to_string(power + 1) + " / (" + std::to_string(a) + " * " +
		std::to_string(power + 1) + ").");
	result.steps.push_back("Vrijednost odredjenog integrala na [" + std::to_string(lowerBound) + ", " +
		std::to_string(upperBound) + "] iznosi " + std::to_string(result.value) + ".");

	return result;
}

} // namespace aksiomat::mathematical_analysis
