#include "aksiomat/linear_algebra/EigenAnalysis.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::linear_algebra {

namespace {

void requireFinite(double value, const char* name) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(std::string(name) + " mora biti konacan broj.");
	}
}

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

std::vector<double> normalize(std::vector<double> vector) {
	double sumSquares = 0.0;
	for (double value : vector) sumSquares += value * value;
	const double length = std::sqrt(sumSquares);
	if (length > 1e-12) {
		for (double& value : vector) value /= length;
	}
	return vector;
}

} // namespace

EigenAnalysisResult EigenAnalysis::analyzeSymmetric2x2(double a11, double a12, double a22) {
	requireFinite(a11, "a11"); requireFinite(a12, "a12"); requireFinite(a22, "a22");

	const double trace = a11 + a22;
	const double det = a11 * a22 - a12 * a12;
	const double discriminant = trace * trace - 4.0 * det;
	if (discriminant < -1e-9) {
		throw std::invalid_argument("Simetricna matrica mora imati realne svojstvene vrijednosti (diskriminanta je negativna).");
	}
	const double sqrtDisc = std::sqrt(std::max(discriminant, 0.0));
	const double lambda1 = (trace + sqrtDisc) / 2.0;
	const double lambda2 = (trace - sqrtDisc) / 2.0;

	std::vector<std::string> steps;
	steps.push_back("Karakteristicni polinom simetricne 2x2 matrice: lambda^2 - trag*lambda + determinanta = 0.");
	steps.push_back("Trag = " + formatNumber(trace) + ", determinanta = " + formatNumber(det) + ".");
	steps.push_back("Svojstvene vrijednosti: lambda1 = " + formatNumber(lambda1) + ", lambda2 = " + formatNumber(lambda2) + ".");

	auto eigenvectorFor = [&](double lambda) {
		std::vector<double> vector;
		if (std::abs(a12) > 1e-12) {
			vector = {lambda - a22, a12};
		} else if (std::abs(a11 - lambda) < 1e-9) {
			vector = {1.0, 0.0};
		} else {
			vector = {0.0, 1.0};
		}
		return normalize(vector);
	};

	std::vector<EigenPair> pairs{
		EigenPair{lambda1, eigenvectorFor(lambda1)},
		EigenPair{lambda2, eigenvectorFor(lambda2)}
	};
	steps.push_back("Svojstveni vektori dobivaju se rjesavanjem (A - lambda*I)v = 0 i normalizacijom.");

	return EigenAnalysisResult{pairs, true, std::move(steps)};
}

EigenAnalysisResult EigenAnalysis::analyzeSymmetric3x3(const std::vector<std::vector<double>>& matrix) {
	if (matrix.size() != 3) {
		throw std::invalid_argument("Matrica mora biti dimenzije 3x3.");
	}
	for (const auto& row : matrix) {
		if (row.size() != 3) {
			throw std::invalid_argument("Matrica mora biti dimenzije 3x3.");
		}
		for (double value : row) {
			if (!std::isfinite(value)) {
				throw std::invalid_argument("Matrica sadrzi nekonacnu vrijednost.");
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		for (int j = i + 1; j < 3; ++j) {
			if (std::abs(matrix[i][j] - matrix[j][i]) > 1e-9) {
				throw std::invalid_argument("Matrica mora biti simetricna (a_ij = a_ji).");
			}
		}
	}

	// Jacobijeva metoda rotacija za simetricne matrice - garantira realnu konvergenciju.
	double a[3][3];
	for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) a[i][j] = matrix[i][j];
	double v[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

	for (int sweep = 0; sweep < 100; ++sweep) {
		double offDiagonalSum = std::abs(a[0][1]) + std::abs(a[0][2]) + std::abs(a[1][2]);
		if (offDiagonalSum < 1e-12) break;
		for (int p = 0; p < 3; ++p) {
			for (int q = p + 1; q < 3; ++q) {
				if (std::abs(a[p][q]) < 1e-15) continue;
				const double theta = (a[q][q] - a[p][p]) / (2.0 * a[p][q]);
				const double sign = theta >= 0 ? 1.0 : -1.0;
				const double t = sign / (std::abs(theta) + std::sqrt(theta * theta + 1.0));
				const double c = 1.0 / std::sqrt(t * t + 1.0);
				const double s = t * c;

				const double app = a[p][p], aqq = a[q][q], apq = a[p][q];
				a[p][p] = c * c * app - 2.0 * s * c * apq + s * s * aqq;
				a[q][q] = s * s * app + 2.0 * s * c * apq + c * c * aqq;
				a[p][q] = 0.0;
				a[q][p] = 0.0;
				for (int k = 0; k < 3; ++k) {
					if (k != p && k != q) {
						const double akp = a[k][p], akq = a[k][q];
						a[k][p] = c * akp - s * akq;
						a[p][k] = a[k][p];
						a[k][q] = s * akp + c * akq;
						a[q][k] = a[k][q];
					}
				}
				for (int k = 0; k < 3; ++k) {
					const double vkp = v[k][p], vkq = v[k][q];
					v[k][p] = c * vkp - s * vkq;
					v[k][q] = s * vkp + c * vkq;
				}
			}
		}
	}

	std::vector<EigenPair> pairs;
	for (int i = 0; i < 3; ++i) {
		std::vector<double> vector = normalize({v[0][i], v[1][i], v[2][i]});
		pairs.push_back(EigenPair{a[i][i], vector});
	}
	std::sort(pairs.begin(), pairs.end(), [](const EigenPair& lhs, const EigenPair& rhs) { return lhs.eigenvalue > rhs.eigenvalue; });

	std::vector<std::string> steps;
	steps.push_back("Simetricna 3x3 matrica dijagonalizirana je Jacobijevom metodom rotacija.");
	for (const auto& pair : pairs) {
		steps.push_back("Svojstvena vrijednost lambda = " + formatNumber(pair.eigenvalue) + " s pripadnim normaliziranim svojstvenim vektorom.");
	}

	return EigenAnalysisResult{pairs, true, std::move(steps)};
}

} // namespace aksiomat::linear_algebra
