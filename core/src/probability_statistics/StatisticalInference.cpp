#include "aksiomat/probability_statistics/StatisticalInference.hpp"

#include <cmath>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace aksiomat::probability_statistics {

namespace {

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

double sampleMean(const std::vector<double>& sample) {
	return std::accumulate(sample.begin(), sample.end(), 0.0) / static_cast<double>(sample.size());
}

double sampleStdDev(const std::vector<double>& sample, double mean) {
	double sumSquares = 0.0;
	for (double value : sample) sumSquares += (value - mean) * (value - mean);
	return std::sqrt(sumSquares / static_cast<double>(sample.size() - 1));
}

// Inverzna standardna normalna kumulativna funkcija (Acklamova racionalna aproksimacija).
double inverseStandardNormalCdf(double probability) {
	if (probability <= 0.0 || probability >= 1.0) {
		throw std::invalid_argument("Vjerojatnost za kvantil mora biti u otvorenom intervalu (0,1).");
	}
	static const double a[] = { -3.969683028665376e+01, 2.209460984245205e+02, -2.759285104469687e+02,
		1.383577518672690e+02, -3.066479806614716e+01, 2.506628277459239e+00 };
	static const double b[] = { -5.447609879822406e+01, 1.615858368580409e+02, -1.556989798598866e+02,
		6.680131188771972e+01, -1.328068155288572e+01 };
	static const double c[] = { -7.784894002430293e-03, -3.223964580411365e-01, -2.400758277161838e+00,
		-2.549732539343734e+00, 4.374664141464968e+00, 2.938163982698783e+00 };
	static const double d[] = { 7.784695709041462e-03, 3.224671290700398e-01, 2.445134137142996e+00,
		3.754408661907416e+00 };

	const double pLow = 0.02425;
	const double pHigh = 1.0 - pLow;
	double q, r;

	if (probability < pLow) {
		q = std::sqrt(-2.0 * std::log(probability));
		return (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
			/ ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
	}
	if (probability <= pHigh) {
		q = probability - 0.5;
		r = q * q;
		return (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) * q
			/ (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1.0);
	}
	q = std::sqrt(-2.0 * std::log(1.0 - probability));
	return -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
		/ ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
}

double standardNormalCdf(double z) {
	return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

void validateSample(const std::vector<double>& sample, std::size_t minimumSize) {
	if (sample.size() < minimumSize) {
		throw std::invalid_argument("Uzorak mora imati barem " + std::to_string(minimumSize) + " vrijednosti.");
	}
}

} // namespace

ConfidenceIntervalResult StatisticalInference::confidenceIntervalForMean(
	const std::vector<double>& sample, double confidenceLevel, double populationStdDev) {
	validateSample(sample, 2);
	if (confidenceLevel <= 0.0 || confidenceLevel >= 1.0) {
		throw std::invalid_argument("Razina pouzdanosti mora biti u (0,1).");
	}
	if (populationStdDev <= 0.0) throw std::invalid_argument("Standardna devijacija mora biti pozitivna.");

	std::vector<std::string> steps;
	const double mean = sampleMean(sample);
	const std::size_t n = sample.size();
	steps.push_back("Uzoracka sredina x̄ = " + formatNumber(mean) + " (n=" + std::to_string(n) + ")");
	const double alpha = 1.0 - confidenceLevel;
	const double zValue = inverseStandardNormalCdf(1.0 - alpha / 2.0);
	steps.push_back("Kriticna vrijednost z(1-alpha/2) za razinu pouzdanosti " + formatNumber(confidenceLevel) + " = " + formatNumber(zValue));
	const double marginOfError = zValue * populationStdDev / std::sqrt(static_cast<double>(n));
	steps.push_back("Granica pogreske E = z * sigma/sqrt(n) = " + formatNumber(marginOfError));
	const double lowerBound = mean - marginOfError;
	const double upperBound = mean + marginOfError;
	steps.push_back("Interval pouzdanosti: (" + formatNumber(lowerBound) + ", " + formatNumber(upperBound) + ")");

	return { mean, marginOfError, lowerBound, upperBound, steps };
}

HypothesisTestResult StatisticalInference::zTestForMean(
	const std::vector<double>& sample, double hypothesizedMean, double populationStdDev, double significanceLevel) {
	validateSample(sample, 2);
	if (populationStdDev <= 0.0) throw std::invalid_argument("Standardna devijacija mora biti pozitivna.");
	if (significanceLevel <= 0.0 || significanceLevel >= 1.0) {
		throw std::invalid_argument("Razina znacajnosti mora biti u (0,1).");
	}

	std::vector<std::string> steps;
	const double mean = sampleMean(sample);
	const std::size_t n = sample.size();
	steps.push_back("H0: mu = " + formatNumber(hypothesizedMean) + ", uzoracka sredina x̄ = " + formatNumber(mean));
	const double testStatistic = (mean - hypothesizedMean) / (populationStdDev / std::sqrt(static_cast<double>(n)));
	steps.push_back("Testna statistika z = (x̄ - mu0) / (sigma/sqrt(n)) = " + formatNumber(testStatistic));
	const double criticalValue = inverseStandardNormalCdf(1.0 - significanceLevel / 2.0);
	steps.push_back("Kriticna vrijednost z(1-alpha/2) = " + formatNumber(criticalValue));
	const bool rejectNull = std::fabs(testStatistic) > criticalValue;
	steps.push_back(rejectNull ? "|z| > kriticne vrijednosti, odbacujemo H0." : "|z| <= kriticne vrijednosti, ne odbacujemo H0.");

	return { testStatistic, criticalValue, rejectNull, steps };
}

RegressionResult StatisticalInference::simpleLinearRegression(const std::vector<double>& x, const std::vector<double>& y) {
	if (x.size() != y.size()) throw std::invalid_argument("Nizovi x i y moraju biti iste duljine.");
	validateSample(x, 2);

	std::vector<std::string> steps;
	const std::size_t n = x.size();
	const double meanX = sampleMean(x);
	const double meanY = sampleMean(y);
	steps.push_back("Srednje vrijednosti: x̄ = " + formatNumber(meanX) + ", ȳ = " + formatNumber(meanY));

	double sumXY = 0.0, sumXX = 0.0, sumYY = 0.0;
	for (std::size_t i = 0; i < n; ++i) {
		const double dx = x[i] - meanX;
		const double dy = y[i] - meanY;
		sumXY += dx * dy;
		sumXX += dx * dx;
		sumYY += dy * dy;
	}
	if (sumXX == 0.0) throw std::invalid_argument("Sve x vrijednosti su jednake, nagib nije definiran.");

	const double slope = sumXY / sumXX;
	const double intercept = meanY - slope * meanX;
	steps.push_back("Nagib b = Sxy/Sxx = " + formatNumber(slope));
	steps.push_back("Odsjecak a = ȳ - b*x̄ = " + formatNumber(intercept));

	const double correlation = (sumXX > 0.0 && sumYY > 0.0) ? sumXY / std::sqrt(sumXX * sumYY) : 0.0;
	const double rSquared = correlation * correlation;
	steps.push_back("Koeficijent korelacije r = " + formatNumber(correlation) + ", koeficijent determinacije r^2 = " + formatNumber(rSquared));
	steps.push_back("Regresijski pravac: y = " + formatNumber(slope) + "x + " + formatNumber(intercept));

	return { slope, intercept, correlation, rSquared, steps };
}

} // namespace aksiomat::probability_statistics
