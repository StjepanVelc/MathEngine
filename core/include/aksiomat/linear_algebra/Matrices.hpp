#pragma once

#include <string>
#include <vector>

namespace aksiomat::linear_algebra {

// Kvadratna matrica pohranjena po recima, podrzane su dimenzije 2 i 3.
using MatrixRows = std::vector<std::vector<double>>;

struct MatrixOperationResult {
	MatrixRows matrix;
	std::vector<std::string> steps;
};

struct DeterminantResult {
	double determinant;
	std::vector<std::string> steps;
};

struct InverseResult {
	bool invertible;
	MatrixRows inverse;
	double determinant;
	std::vector<std::string> steps;
};

struct TransformationResult {
	std::vector<double> transformedVector;
	std::vector<std::string> steps;
};

class Matrices {
public:
	static MatrixOperationResult add(const MatrixRows& left, const MatrixRows& right);
	static MatrixOperationResult multiply(const MatrixRows& left, const MatrixRows& right);
	static DeterminantResult determinant(const MatrixRows& matrix);
	static InverseResult inverse(const MatrixRows& matrix);
	static TransformationResult applyTransformation(const MatrixRows& matrix, const std::vector<double>& vector);
};

} // namespace aksiomat::linear_algebra
