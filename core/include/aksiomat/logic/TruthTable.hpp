#pragma once

#include <string>
#include <vector>

#include "aksiomat/logic/LogicExpression.hpp"

namespace aksiomat {

// Generiranje tablice istinitosti i provjera ekvivalencije formula.
class TruthTable {
public:
	// Maksimalan broj varijabli u formuli (2^N redaka).
	static constexpr std::size_t kMaxVariables = 6;

	struct Row {
		LogicExpression::Valuation valuation;
		bool result = false;
	};

	struct Table {
		std::vector<std::string> variables; // abecedno sortirane
		std::vector<Row> rows;              // 2^N redaka
	};

	// Generira tablicu istinitosti za izraz.
	// Baca std::invalid_argument ako izraz ima vise od kMaxVariables varijabli.
	static Table generate(const LogicExpression::Ptr& expr);

	// Provjerava jesu li dvije formule logicki ekvivalentne
	// (jednake na svim valuacijama nad unijom njihovih varijabli).
	static bool areEquivalent(const LogicExpression::Ptr& lhs, const LogicExpression::Ptr& rhs);
};

} // namespace aksiomat
