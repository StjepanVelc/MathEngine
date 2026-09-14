#include "aksiomat/linear_algebra/Matrices.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::linear_algebra {

namespace {

void validateSquare(const MatrixRows& matrix, const char* label) {
	if (matrix.empty()) {
		throw std::invalid_argument(std::string(label) + " ne smije biti prazna.");
	}
	const std::size_t size = matrix.size();
	if (size != 2 && size != 3) {
		throw std::invalid_argument(std::string(label) + " mora biti kvadratna matrica dimenzije 2x2 ili 3x3.");
	}
	for (const auto& row : matrix) {
		if (row.size() != size) {
			throw std::invalid_argument(std::string(label) + " mora biti kvadratna matrica.");
		}
		for (double value : row) {
			if (!std::isfinite(value)) {
				throw std::invalid_argument(std::string(label) + " sadrzi nekonacnu vrijednost.");
			}
		}
	}
}

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

double determinant2x2(const MatrixRows& m) {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

double determinant3x3(const MatrixRows& m) {
	return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
		- m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
		+ m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

} // namespace

MatrixOperationResult Matrices::add(const MatrixRows& left, const MatrixRows& right) {
	validateSquare(left, "Prva matrica");
	validateSquare(right, "Druga matrica");
	if (left.size() != right.size()) {
		throw std::invalid_argument("Matrice moraju biti iste dimenzije za zbrajanje.");
	}
	const std::size_t n = left.size();
	MatrixRows result(n, std::vector<double>(n, 0.0));
	for (std::size_t i = 0; i < n; ++i) {
		for (std::size_t j = 0; j < n; ++j) {
			result[i][j] = left[i][j] + right[i][j];
		}
	}
	std::vector<std::string> steps{"Zbrajanje matrica provodi se element po element."};
	return MatrixOperationResult{result, std::move(steps)};
}

MatrixOperationResult Matrices::multiply(const MatrixRows& left, const MatrixRows& right) {
	validateSquare(left, "Prva matrica");
	validateSquare(right, "Druga matrica");
	if (left.size() != right.size()) {
		throw std::invalid_argument("Matrice moraju biti iste dimenzije za mnozenje.");
	}
	const std::size_t n = left.size();
	MatrixRows result(n, std::vector<double>(n, 0.0));
	for (std::size_t i = 0; i < n; ++i) {
		for (std::size_t j = 0; j < n; ++j) {
			double sum = 0.0;
			for (std::size_t k = 0; k < n; ++k) {
				sum += left[i][k] * right[k][j];
			}
			result[i][j] = sum;
		}
	}
	std::vector<std::string> steps{"Mnozenje matrica: element (i,j) rezultata je skalarni produkt i-tog retka prve i j-tog stupca druge matrice."};
	return MatrixOperationResult{result, std::move(steps)};
}

DeterminantResult Matrices::determinant(const MatrixRows& matrix) {
	validateSquare(matrix, "Matrica");
	const double value = matrix.size() == 2 ? determinant2x2(matrix) : determinant3x3(matrix);
	std::vector<std::string> steps;
	if (matrix.size() == 2) {
		steps.push_back("Determinanta 2x2 matrice: a11*a22 - a12*a21 = " + formatNumber(value) + ".");
	} else {
		steps.push_back("Determinanta 3x3 matrice racuna se Sarrusovim pravilom ili razvojem po prvom retku: " + formatNumber(value) + ".");
	}
	return DeterminantResult{value, std::move(steps)};
}

InverseResult Matrices::inverse(const MatrixRows& matrix) {
	validateSquare(matrix, "Matrica");
	const double det = matrix.size() == 2 ? determinant2x2(matrix) : determinant3x3(matrix);
	std::vector<std::string> steps;
	steps.push_back("Determinanta matrice iznosi " + formatNumber(det) + ".");
	if (std::abs(det) < 1e-12) {
		steps.push_back("Determinanta je priblizno nula, matrica nije invertibilna.");
		return InverseResult{false, {}, det, std::move(steps)};
	}

	MatrixRows inv;
	if (matrix.size() == 2) {
		inv = {
			{matrix[1][1] / det, -matrix[0][1] / det},
			{-matrix[1][0] / det, matrix[0][0] / det}
		};
		steps.push_back("Za 2x2 matricu inverz je adjunkta podijeljena determinantom: swap dijagonale i promjena predznaka izvandijagonalnih clanova.");
	} else {
		MatrixRows cofactor(3, std::vector<double>(3, 0.0));
		auto minor = [&matrix](int row, int col) {
			double values[4];
			int index = 0;
			for (int i = 0; i < 3; ++i) {
				if (i == row) continue;
				for (int j = 0; j < 3; ++j) {
					if (j == col) continue;
					values[index++] = matrix[i][j];
				}
			}
			return values[0] * values[3] - values[1] * values[2];
		};
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				const double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
				cofactor[i][j] = sign * minor(i, j);
			}
		}
		inv = {
			{cofactor[0][0] / det, cofactor[1][0] / det, cofactor[2][0] / det},
			{cofactor[0][1] / det, cofactor[1][1] / det, cofactor[2][1] / det},
			{cofactor[0][2] / det, cofactor[1][2] / det, cofactor[2][2] / det}
		};
		steps.push_back("Za 3x3 matricu inverz je transponirana matrica kofaktora podijeljena determinantom.");
	}
	return InverseResult{true, inv, det, std::move(steps)};
}

TransformationResult Matrices::applyTransformation(const MatrixRows& matrix, const std::vector<double>& vector) {
	validateSquare(matrix, "Matrica");
	if (vector.size() != matrix.size()) {
		throw std::invalid_argument("Vektor mora imati istu dimenziju kao matrica.");
	}
	for (double value : vector) {
		if (!std::isfinite(value)) {
			throw std::invalid_argument("Vektor sadrzi nekonacnu vrijednost.");
		}
	}
	const std::size_t n = matrix.size();
	std::vector<double> result(n, 0.0);
	for (std::size_t i = 0; i < n; ++i) {
		double sum = 0.0;
		for (std::size_t j = 0; j < n; ++j) {
			sum += matrix[i][j] * vector[j];
		}
		result[i] = sum;
	}
	std::vector<std::string> steps{"Primjena linearne transformacije: novi vektor = matrica * originalni vektor."};
	return TransformationResult{result, std::move(steps)};
}

} // namespace aksiomat::linear_algebra
