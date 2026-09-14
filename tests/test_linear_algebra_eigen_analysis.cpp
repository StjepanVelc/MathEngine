#include "aksiomat/linear_algebra/EigenAnalysis.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::linear_algebra;

TEST(EigenAnalysis, ComputesSymmetric2x2Eigenvalues) {
	// Matrica [[2,0],[0,3]] ima svojstvene vrijednosti 2 i 3.
	EigenAnalysisResult result = EigenAnalysis::analyzeSymmetric2x2(2, 0, 3);
	ASSERT_EQ(result.eigenPairs.size(), 2u);
	EXPECT_TRUE(result.allRealEigenvalues);
	EXPECT_NEAR(result.eigenPairs[0].eigenvalue, 3.0, 1e-9);
	EXPECT_NEAR(result.eigenPairs[1].eigenvalue, 2.0, 1e-9);
}

TEST(EigenAnalysis, ComputesSymmetric2x2WithOffDiagonal) {
	EigenAnalysisResult result = EigenAnalysis::analyzeSymmetric2x2(2, 1, 2);
	ASSERT_EQ(result.eigenPairs.size(), 2u);
	EXPECT_NEAR(result.eigenPairs[0].eigenvalue, 3.0, 1e-9);
	EXPECT_NEAR(result.eigenPairs[1].eigenvalue, 1.0, 1e-9);
}

TEST(EigenAnalysis, ComputesSymmetric3x3Identity) {
	std::vector<std::vector<double>> m{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	EigenAnalysisResult result = EigenAnalysis::analyzeSymmetric3x3(m);
	ASSERT_EQ(result.eigenPairs.size(), 3u);
	for (const auto& pair : result.eigenPairs) {
		EXPECT_NEAR(pair.eigenvalue, 1.0, 1e-6);
	}
}

TEST(EigenAnalysis, ComputesSymmetric3x3Diagonal) {
	std::vector<std::vector<double>> m{{1, 0, 0}, {0, 2, 0}, {0, 0, 3}};
	EigenAnalysisResult result = EigenAnalysis::analyzeSymmetric3x3(m);
	ASSERT_EQ(result.eigenPairs.size(), 3u);
	EXPECT_NEAR(result.eigenPairs[0].eigenvalue, 3.0, 1e-6);
	EXPECT_NEAR(result.eigenPairs[1].eigenvalue, 2.0, 1e-6);
	EXPECT_NEAR(result.eigenPairs[2].eigenvalue, 1.0, 1e-6);
}

TEST(EigenAnalysis, RejectsAsymmetricMatrix) {
	std::vector<std::vector<double>> m{{1, 2, 0}, {0, 1, 0}, {0, 0, 1}};
	EXPECT_THROW(EigenAnalysis::analyzeSymmetric3x3(m), std::invalid_argument);
}
