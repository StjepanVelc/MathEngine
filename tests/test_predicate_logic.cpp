#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/predicate/Interpretation.hpp"
#include "aksiomat/predicate/PredicateExpression.hpp"
#include "aksiomat/predicate/PredicateParser.hpp"

using aksiomat::Interpretation;
using aksiomat::PredicateParser;

namespace {

Interpretation makeInterpretation() {
	Interpretation interp;
	interp.setDomain({"1", "2", "3"});
	interp.addFact("Paran", {"2"});
	interp.addFact("Manji", {"1", "2"});
	interp.addFact("Manji", {"1", "3"});
	interp.addFact("Manji", {"2", "3"});
	return interp;
}

TEST(PredicateParser, ParsesEqualityAndInequality) {
	EXPECT_EQ(PredicateParser::parse("x = y")->toString(), "x = y");
	EXPECT_EQ(PredicateParser::parse("x != y")->toString(), "x \u2260 y");
	EXPECT_EQ(PredicateParser::parse("x \u2260 y")->toString(), "x \u2260 y");
}

TEST(PredicateParser, EqualityIsAnAtomicFormula) {
	auto expression = PredicateParser::parse("forall x (x = x | P(x))");
	EXPECT_EQ(expression->toString(), "\u2200x (x = x \u2228 P(x))");
}

TEST(PredicateParser, CollectsFreeAndBoundVariablesFromEquality) {
	auto expression = PredicateParser::parse("forall x (x = y & exists z z != x)");
	EXPECT_EQ(PredicateParser::collectFreeVariables(expression), (std::set<std::string>{"y"}));
	EXPECT_EQ(PredicateParser::collectBoundVariables(expression),
			  (std::set<std::string>{"x", "z"}));
}

TEST(PredicateParser, RejectsConflictingPredicateArities) {
	EXPECT_THROW(PredicateParser::parse("P(x) & P(x, y)"), std::invalid_argument);
}

TEST(Interpretation, EvaluatesEqualityAndInequality) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	EXPECT_TRUE(interpretation.evaluate(PredicateParser::parse("1 = 1")));
	EXPECT_FALSE(interpretation.evaluate(PredicateParser::parse("1 = 2")));
	EXPECT_TRUE(interpretation.evaluate(PredicateParser::parse("1 != 2")));
	EXPECT_TRUE(interpretation.evaluate(PredicateParser::parse("forall x x = x")));
	EXPECT_TRUE(interpretation.evaluate(PredicateParser::parse("forall x exists y x = y")));
}

TEST(Interpretation, EqualityRejectsUnknownFreeTerm) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	EXPECT_THROW(interpretation.evaluate(PredicateParser::parse("x = 1")),
				 std::invalid_argument);
}

TEST(PredicateParser, HandlesShadowedVariables) {
	auto expression = PredicateParser::parse("forall x (P(x) & exists x Q(x))");
	EXPECT_TRUE(PredicateParser::collectFreeVariables(expression).empty());
	EXPECT_EQ(expression->toString(), "\u2200x (P(x) \u2227 \u2203x Q(x))");
}

} // namespace

TEST(PredicateParser, ParsesUnicodeQuantifiers) {
	auto expr = PredicateParser::parse("\u2200x \u2203y Manji(x, y)");
	EXPECT_EQ(expr->toString(), "\u2200x \u2203y Manji(x, y)");
}

TEST(Interpretation, RejectsEmptyDomain) {
	Interpretation interpretation;
	EXPECT_THROW(interpretation.setDomain({}), std::invalid_argument);
}

TEST(Interpretation, RejectsDuplicateDomainElements) {
	Interpretation interpretation;
	EXPECT_THROW(interpretation.setDomain({"1", "1"}), std::invalid_argument);
}

TEST(Interpretation, RejectsFactOutsideDomain) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	EXPECT_THROW(interpretation.addFact("P", {"3"}), std::invalid_argument);
}

TEST(Interpretation, IgnoresUnrelatedPredicateFact) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	interpretation.addFact("Q", {"1"});
	EXPECT_FALSE(interpretation.evaluate(PredicateParser::parse("exists x P(x)")));
}

TEST(Interpretation, RejectsFactWithWrongArity) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	interpretation.addFact("P", {"1", "2"});
	EXPECT_THROW(interpretation.evaluate(PredicateParser::parse("exists x P(x)")),
				 std::invalid_argument);
}

TEST(Interpretation, RejectsMixedFactAritiesForSamePredicate) {
	Interpretation interpretation;
	interpretation.setDomain({"1", "2"});
	interpretation.addFact("P", {"1"});
	EXPECT_THROW(interpretation.addFact("P", {"1", "2"}), std::invalid_argument);
}

TEST(Interpretation, RejectsEvaluationBeforeDomainIsSet) {
	Interpretation interpretation;
	EXPECT_THROW(interpretation.evaluate(PredicateParser::parse("P(x)")), std::logic_error);
}

TEST(PredicateParser, ParsesAsciiKeywords) {
	auto expr = PredicateParser::parse("forall x exists y Manji(x, y)");
	EXPECT_EQ(expr->toString(), "\u2200x \u2203y Manji(x, y)");
}

TEST(PredicateParser, ParsesConnectives) {
	auto expr = PredicateParser::parse("forall x (Paran(x) -> !Neparan(x))");
	EXPECT_EQ(expr->toString(), "\u2200x (Paran(x) \u2192 \u00ACNeparan(x))");
}

TEST(PredicateParser, CollectFreeVariables) {
	auto expr = PredicateParser::parse("forall x Manji(x, y)");
	EXPECT_EQ(PredicateParser::collectFreeVariables(expr), (std::set<std::string>{"y"}));

	auto closed = PredicateParser::parse("forall x exists y Manji(x, y)");
	EXPECT_TRUE(PredicateParser::collectFreeVariables(closed).empty());
}

TEST(PredicateParser, CollectPredicates) {
	auto expr = PredicateParser::parse("forall x (Paran(x) | Manji(x, x))");
	auto predicates = PredicateParser::collectPredicates(expr);
	EXPECT_EQ(predicates.at("Paran"), 1u);
	EXPECT_EQ(predicates.at("Manji"), 2u);
}

TEST(PredicateParser, InvalidFormulaThrows) {
	EXPECT_THROW(PredicateParser::parse(""), std::invalid_argument);
	EXPECT_THROW(PredicateParser::parse("P"), std::invalid_argument);
	EXPECT_THROW(PredicateParser::parse("P(x"), std::invalid_argument);
	EXPECT_THROW(PredicateParser::parse("forall P(x)"), std::invalid_argument);
	EXPECT_THROW(PredicateParser::parse("P(x) &"), std::invalid_argument);
}

TEST(Interpretation, EvaluatesExistential) {
	auto interp = makeInterpretation();
	EXPECT_TRUE(interp.evaluate(PredicateParser::parse("exists x Paran(x)")));
	EXPECT_FALSE(interp.evaluate(PredicateParser::parse("exists x Manji(x, x)")));
}

TEST(Interpretation, EvaluatesUniversal) {
	auto interp = makeInterpretation();
	EXPECT_FALSE(interp.evaluate(PredicateParser::parse("forall x Paran(x)")));
	// Svaki element osim 3 je manji od necega.
	EXPECT_FALSE(interp.evaluate(PredicateParser::parse("forall x exists y Manji(x, y)")));
	EXPECT_TRUE(interp.evaluate(PredicateParser::parse("exists y forall x (Manji(x, y) | Paran(x) | Manji(y, x) | !Manji(x, x))")));
}

TEST(Interpretation, NestedQuantifiers) {
	auto interp = makeInterpretation();
	// Postoji najmanji element: ∃x ∀y (¬Manji(y, x))
	EXPECT_TRUE(interp.evaluate(PredicateParser::parse("exists x forall y !Manji(y, x)")));
}

TEST(Interpretation, ConstantsFromDomain) {
	auto interp = makeInterpretation();
	EXPECT_TRUE(interp.evaluate(PredicateParser::parse("Manji(1, 2)")));
	EXPECT_FALSE(interp.evaluate(PredicateParser::parse("Manji(2, 1)")));
}

TEST(Interpretation, FreeVariableNotInDomainThrows) {
	auto interp = makeInterpretation();
	EXPECT_THROW(interp.evaluate(PredicateParser::parse("Paran(z)")), std::invalid_argument);
}
