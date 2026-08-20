#include <gtest/gtest.h>

#include <set>

#include "aksiomat/LogicParser.hpp"
#include "aksiomat/NormalForms.hpp"
#include "aksiomat/TruthTable.hpp"

using aksiomat::LogicExpression;
using aksiomat::LogicOp;
using aksiomat::LogicParser;
using aksiomat::NormalForms;
using aksiomat::TruthTable;

namespace {

bool isLiteral(const LogicExpression::Ptr& expression) {
	return expression->op() == LogicOp::Variable || expression->op() == LogicOp::Constant ||
		   (expression->op() == LogicOp::Not && expression->left()->op() == LogicOp::Variable);
}

bool isNnf(const LogicExpression::Ptr& expression) {
	if (isLiteral(expression)) return true;
	if (expression->op() != LogicOp::And && expression->op() != LogicOp::Or) return false;
	return isNnf(expression->left()) && isNnf(expression->right());
}

bool isClause(const LogicExpression::Ptr& expression) {
	return isLiteral(expression) ||
		   (expression->op() == LogicOp::Or && isClause(expression->left()) && isClause(expression->right()));
}

bool isCnf(const LogicExpression::Ptr& expression) {
	return isClause(expression) ||
		   (expression->op() == LogicOp::And && isCnf(expression->left()) && isCnf(expression->right()));
}

bool isTerm(const LogicExpression::Ptr& expression) {
	return isLiteral(expression) ||
		   (expression->op() == LogicOp::And && isTerm(expression->left()) && isTerm(expression->right()));
}

bool isDnf(const LogicExpression::Ptr& expression) {
	return isTerm(expression) ||
		   (expression->op() == LogicOp::Or && isDnf(expression->left()) && isDnf(expression->right()));
}

void expectEquivalent(const LogicExpression::Ptr& lhs, const LogicExpression::Ptr& rhs) {
	EXPECT_TRUE(TruthTable::areEquivalent(lhs, rhs));
}

} // namespace

TEST(NormalForms, NnfEliminatesImplicationAndPushesNegation) {
	auto original = LogicParser::parse("!(p -> (q & !r))");
	auto result = NormalForms::toNnf(original);
	EXPECT_TRUE(isNnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, NnfEliminatesEquivalence) {
	auto original = LogicParser::parse("p <-> q");
	auto result = NormalForms::toNnf(original);
	EXPECT_TRUE(isNnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, TransformedCnfIsEquivalent) {
	auto original = LogicParser::parse("p | (q & r)");
	auto result = NormalForms::toCnf(original);
	EXPECT_TRUE(isCnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, TransformedDnfIsEquivalent) {
	auto original = LogicParser::parse("p & (q | r)");
	auto result = NormalForms::toDnf(original);
	EXPECT_TRUE(isDnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, CanonicalCnfIsEquivalent) {
	auto original = LogicParser::parse("p -> q");
	auto result = NormalForms::toCanonicalCnf(original);
	EXPECT_TRUE(isCnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, CanonicalDnfIsEquivalent) {
	auto original = LogicParser::parse("p -> q");
	auto result = NormalForms::toCanonicalDnf(original);
	EXPECT_TRUE(isDnf(result));
	expectEquivalent(original, result);
}

TEST(NormalForms, CanonicalFormsHandleConstants) {
	expectEquivalent(LogicParser::parse("T"), NormalForms::toCanonicalCnf(LogicParser::parse("T")));
	expectEquivalent(LogicParser::parse("F"), NormalForms::toCanonicalDnf(LogicParser::parse("F")));
}
