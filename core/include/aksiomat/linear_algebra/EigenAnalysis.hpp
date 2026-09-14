#pragma once

#include <string>
#include <vector>

namespace aksiomat::linear_algebra {

struct EigenPair {
	double eigenvalue;
	std::vector<double> eigenvector;
};

struct EigenAnalysisResult {
	std::vector<EigenPair> eigenPairs;
	bool allRealEigenvalues;
	std::vector<std::string> steps;
};

class EigenAnalysis {
public:
	// Podrzava simetricne 2x2 i 3x3 matrice zadane po recima (garantira realne svojstvene vrijednosti).
	static EigenAnalysisResult analyzeSymmetric2x2(double a11, double a12, double a22);
	static EigenAnalysisResult analyzeSymmetric3x3(const std::vector<std::vector<double>>& matrix);
};

} // namespace aksiomat::linear_algebra
