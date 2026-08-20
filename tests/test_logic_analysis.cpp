#include <gtest/gtest.h>

#include "aksiomat/LogicAnalysis.hpp"
#include "aksiomat/LogicParser.hpp"

using aksiomat::LogicAnalysis;
using aksiomat::LogicParser;

TEST(LogicAnalysis, ClassifiesTautology) {
	const auto result = LogicAnalysis::classify(LogicParser::parse("p | !p"));
	EXPECT_TRUE(result.tautology);
	EXPECT_TRUE(result.satisfiable);
	EXPECT_FALSE(result.contradiction);
	EXPECT_FALSE(result.contingent);
}

TEST(LogicAnalysis, ClassifiesContradiction) {
	const auto result = LogicAnalysis::classify(LogicParser::parse("p & !p"));
	EXPECT_TRUE(result.contradiction);
	EXPECT_FALSE(result.satisfiable);
	EXPECT_FALSE(result.tautology);
	EXPECT_FALSE(result.contingent);
}

TEST(LogicAnalysis, ClassifiesContingentFormula) {
	const auto result = LogicAnalysis::classify(LogicParser::parse("p -> q"));
	EXPECT_TRUE(result.satisfiable);
	EXPECT_TRUE(result.contingent);
	EXPECT_FALSE(result.tautology);
	EXPECT_FALSE(result.contradiction);
}

TEST(LogicAnalysis, ClassifiesConstants) {
	EXPECT_TRUE(LogicAnalysis::classify(LogicParser::parse("T")).tautology);
	EXPECT_TRUE(LogicAnalysis::classify(LogicParser::parse("F")).contradiction);
}
