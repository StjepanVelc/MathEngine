#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/PredicateLogic.hpp"

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

} // namespace

TEST(PredicateParser, ParsesUnicodeQuantifiers) {
	auto expr = PredicateParser::parse("\u2200x \u2203y Manji(x, y)");
	EXPECT_EQ(expr->toString(), "\u2200x \u2203y Manji(x, y)");
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
