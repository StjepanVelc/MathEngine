#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/logic/LogicParser.hpp"
#include "aksiomat/logic/TruthTable.hpp"

using aksiomat::LogicParser;
using aksiomat::TruthTable;

TEST(TruthTable, SingleVariable) {
	auto table = TruthTable::generate(LogicParser::parse("!p"));
	ASSERT_EQ(table.variables, (std::vector<std::string>{"p"}));
	ASSERT_EQ(table.rows.size(), 2u);
	EXPECT_TRUE(table.rows[0].result);  // p=F -> ¬p=T
	EXPECT_FALSE(table.rows[1].result); // p=T -> ¬p=F
}

TEST(TruthTable, ConjunctionTable) {
	auto table = TruthTable::generate(LogicParser::parse("p & q"));
	ASSERT_EQ(table.rows.size(), 4u);
	// Poredak: FF, FT, TF, TT
	EXPECT_FALSE(table.rows[0].result);
	EXPECT_FALSE(table.rows[1].result);
	EXPECT_FALSE(table.rows[2].result);
	EXPECT_TRUE(table.rows[3].result);
}

TEST(TruthTable, TautologyAllTrue) {
	auto table = TruthTable::generate(LogicParser::parse("(p -> q) <-> (!p | q)"));
	for (const auto& row : table.rows) {
		EXPECT_TRUE(row.result);
	}
}

TEST(TruthTable, ConstantFormulaHasOneRow) {
	auto table = TruthTable::generate(LogicParser::parse("T & F"));
	ASSERT_EQ(table.rows.size(), 1u);
	EXPECT_FALSE(table.rows[0].result);
}

TEST(TruthTable, TooManyVariablesThrows) {
	EXPECT_THROW(TruthTable::generate(LogicParser::parse("a & b & c & d & e & f & g")),
				 std::invalid_argument);
}

TEST(TruthTable, EquivalentFormulas) {
	// De Morgan
	EXPECT_TRUE(TruthTable::areEquivalent(LogicParser::parse("!(p & q)"),
										  LogicParser::parse("!p | !q")));
	// Kontrapozicija
	EXPECT_TRUE(TruthTable::areEquivalent(LogicParser::parse("p -> q"),
										  LogicParser::parse("!q -> !p")));
}

TEST(TruthTable, NonEquivalentFormulas) {
	EXPECT_FALSE(TruthTable::areEquivalent(LogicParser::parse("p -> q"),
										   LogicParser::parse("q -> p")));
}

TEST(TruthTable, EquivalenceWithDifferentVariableSets) {
	// p ∧ q ≢ p (razlikuju se kad p=T, q=F)
	EXPECT_FALSE(TruthTable::areEquivalent(LogicParser::parse("p & q"),
										   LogicParser::parse("p")));
	// p ∨ ¬p ≡ T (tautologija ekvivalentna konstanti)
	EXPECT_TRUE(TruthTable::areEquivalent(LogicParser::parse("p | !p"),
										  LogicParser::parse("T")));
}
