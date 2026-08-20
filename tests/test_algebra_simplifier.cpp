#include <gtest/gtest.h>

#include "aksiomat/algebra/AlgebraFormatter.hpp"
#include "aksiomat/algebra/AlgebraParser.hpp"
#include "aksiomat/algebra/AlgebraSimplifier.hpp"

using aksiomat::algebra::AlgebraFormatter;
using aksiomat::algebra::AlgebraParser;
using aksiomat::algebra::AlgebraSimplifier;

TEST(AlgebraSimplifier, FoldsConstantsAndNeutralElements) {
	auto result = AlgebraSimplifier::simplify(AlgebraParser::parse("0 + x*1 + 2*3"));
	EXPECT_EQ(AlgebraFormatter::format(result.expression), "x + 6");
	EXPECT_FALSE(result.steps.empty());
}

TEST(AlgebraSimplifier, CombinesCompatibleTerms) {
	auto result = AlgebraSimplifier::simplify(AlgebraParser::parse("2x + 3x - x + 5 - 2"));
	EXPECT_EQ(AlgebraFormatter::format(result.expression), "4 * x + 3");
}

TEST(AlgebraSimplifier, NormalizesUnarySigns) {
	EXPECT_EQ(AlgebraFormatter::format(AlgebraSimplifier::simplify(AlgebraParser::parse("--x")).expression), "x");
	EXPECT_EQ(AlgebraFormatter::format(AlgebraSimplifier::simplify(AlgebraParser::parse("-(2+3)")).expression), "-5");
}

TEST(AlgebraFormatter, PreservesRequiredParentheses) {
	EXPECT_EQ(AlgebraFormatter::format(AlgebraParser::parse("(x+1)*(x-1)")), "(x + 1) * (x - 1)");
}
