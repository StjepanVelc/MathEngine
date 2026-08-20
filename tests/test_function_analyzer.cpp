#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/algebra/FunctionAnalyzer.hpp"

using aksiomat::algebra::FunctionAnalyzer;

TEST(FunctionAnalyzer, AnalyzesLinearFunction) {
	const auto result = FunctionAnalyzer::analyze("2x - 4", -2.0, 2.0, 5);
	EXPECT_EQ(result.normalized, "2x - 4");
	EXPECT_EQ(result.domain, "R");
	EXPECT_EQ(result.degree, 1);
	EXPECT_EQ(result.behavior, "rastuca");
	ASSERT_EQ(result.xIntercepts.size(), 1u);
	EXPECT_DOUBLE_EQ(result.xIntercepts[0].x, 2.0);
	EXPECT_DOUBLE_EQ(result.yIntercept.y, -4.0);
	EXPECT_EQ(result.samples.size(), 5u);
}

TEST(FunctionAnalyzer, AnalyzesQuadraticFunction) {
	const auto result = FunctionAnalyzer::analyze("x^2 - 4x + 3");
	ASSERT_TRUE(result.vertex.has_value());
	EXPECT_DOUBLE_EQ(result.vertex->x, 2.0);
	EXPECT_DOUBLE_EQ(result.vertex->y, -1.0);
	ASSERT_EQ(result.xIntercepts.size(), 2u);
	EXPECT_DOUBLE_EQ(result.xIntercepts[0].x, 1.0);
	EXPECT_DOUBLE_EQ(result.xIntercepts[1].x, 3.0);
}

TEST(FunctionAnalyzer, ValidatesSamplingRange) {
	EXPECT_THROW(FunctionAnalyzer::analyze("x", 1.0, -1.0), std::invalid_argument);
	EXPECT_THROW(FunctionAnalyzer::analyze("x", -1.0, 1.0, 1), std::invalid_argument);
}
