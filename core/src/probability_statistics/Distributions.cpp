#include "aksiomat/probability_statistics/Distributions.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::probability_statistics {

namespace {

constexpr double kPi = 3.14159265358979323846;

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

double factorial(int n) {
	double result = 1.0;
	for (int i = 2; i <= n; ++i) result *= i;
	return result;
}

double binomialCoefficient(int n, int k) {
	return factorial(n) / (factorial(k) * factorial(n - k));
}

// Standardna normalna kumulativna funkcija distribucije preko erf.
double standardNormalCdf(double z) {
	return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

} // namespace

BinomialResult Distributions::binomial(int trials, double probability, int successes) {
	if (trials < 0) throw std::invalid_argument("Broj pokusa mora biti nenegativan.");
	if (probability < 0.0 || probability > 1.0) throw std::invalid_argument("Vjerojatnost mora biti u [0,1].");
	if (successes < 0 || successes > trials) throw std::invalid_argument("Broj uspjeha mora biti izmedu 0 i broja pokusa.");

	std::vector<std::string> steps;
	steps.push_back("Binomna razdioba: X ~ B(n=" + std::to_string(trials) + ", p=" + formatNumber(probability) + ")");
	const double coefficient = binomialCoefficient(trials, successes);
	steps.push_back("Broj kombinacija C(" + std::to_string(trials) + "," + std::to_string(successes) + ") = " + formatNumber(coefficient));
	const double probabilityValue = coefficient * std::pow(probability, successes) * std::pow(1.0 - probability, trials - successes);
	steps.push_back("P(X=" + std::to_string(successes) + ") = C(n,k) * p^k * (1-p)^(n-k) = " + formatNumber(probabilityValue));
	const double mean = trials * probability;
	const double variance = trials * probability * (1.0 - probability);
	steps.push_back("Ocekivanje E(X) = n*p = " + formatNumber(mean) + ", varijanca D(X) = n*p*(1-p) = " + formatNumber(variance));

	return { probabilityValue, mean, variance, steps };
}

PoissonResult Distributions::poisson(double lambda, int occurrences) {
	if (lambda <= 0.0) throw std::invalid_argument("Parametar lambda mora biti pozitivan.");
	if (occurrences < 0) throw std::invalid_argument("Broj dogadaja mora biti nenegativan.");

	std::vector<std::string> steps;
	steps.push_back("Poissonova razdioba: X ~ Po(lambda=" + formatNumber(lambda) + ")");
	const double probabilityValue = std::pow(lambda, occurrences) * std::exp(-lambda) / factorial(occurrences);
	steps.push_back("P(X=" + std::to_string(occurrences) + ") = lambda^k * e^(-lambda) / k! = " + formatNumber(probabilityValue));
	steps.push_back("Ocekivanje E(X) = lambda = " + formatNumber(lambda) + ", varijanca D(X) = lambda = " + formatNumber(lambda));

	return { probabilityValue, lambda, lambda, steps };
}

NormalResult Distributions::normal(double mean, double standardDeviation, double value) {
	if (standardDeviation <= 0.0) throw std::invalid_argument("Standardna devijacija mora biti pozitivna.");

	std::vector<std::string> steps;
	steps.push_back("Normalna razdioba: X ~ N(mu=" + formatNumber(mean) + ", sigma=" + formatNumber(standardDeviation) + ")");
	const double zScore = (value - mean) / standardDeviation;
	steps.push_back("Standardizacija: z = (x - mu) / sigma = " + formatNumber(zScore));
	const double density = std::exp(-0.5 * zScore * zScore) / (standardDeviation * std::sqrt(2.0 * kPi));
	steps.push_back("Gustoca f(x) = " + formatNumber(density));
	const double cumulative = standardNormalCdf(zScore);
	steps.push_back("Kumulativna vjerojatnost P(X<=x) = Phi(z) = " + formatNumber(cumulative));

	return { density, cumulative, zScore, steps };
}

UniformResult Distributions::uniform(double lowerBound, double upperBound, double value) {
	if (upperBound <= lowerBound) throw std::invalid_argument("Gornja granica mora biti veca od donje.");

	std::vector<std::string> steps;
	steps.push_back("Uniformna razdioba: X ~ U(a=" + formatNumber(lowerBound) + ", b=" + formatNumber(upperBound) + ")");
	const double density = (value >= lowerBound && value <= upperBound) ? 1.0 / (upperBound - lowerBound) : 0.0;
	steps.push_back("Gustoca f(x) = 1/(b-a) na [a,b], izvan intervala 0. f(x) = " + formatNumber(density));
	double cumulative;
	if (value < lowerBound) cumulative = 0.0;
	else if (value > upperBound) cumulative = 1.0;
	else cumulative = (value - lowerBound) / (upperBound - lowerBound);
	steps.push_back("Kumulativna vjerojatnost F(x) = " + formatNumber(cumulative));
	const double mean = (lowerBound + upperBound) / 2.0;
	const double variance = std::pow(upperBound - lowerBound, 2) / 12.0;
	steps.push_back("Ocekivanje E(X) = (a+b)/2 = " + formatNumber(mean) + ", varijanca D(X) = (b-a)^2/12 = " + formatNumber(variance));

	return { density, cumulative, mean, variance, steps };
}

} // namespace aksiomat::probability_statistics
