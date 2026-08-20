#include "aksiomat/TruthTable.hpp"

#include <set>
#include <stdexcept>

#include "aksiomat/LogicParser.hpp"

namespace aksiomat {

namespace {

std::vector<std::string> sortedVariables(const std::set<std::string>& names) {
	return {names.begin(), names.end()};
}

LogicExpression::Valuation makeValuation(const std::vector<std::string>& variables, std::size_t mask) {
	LogicExpression::Valuation valuation;
	for (std::size_t i = 0; i < variables.size(); ++i) {
		// Prva varijabla je najznacajniji bit, pa tablica ide F..F do T..T.
		const std::size_t bit = variables.size() - 1 - i;
		valuation[variables[i]] = (mask >> bit) & 1u;
	}
	return valuation;
}

} // namespace

TruthTable::Table TruthTable::generate(const LogicExpression::Ptr& expr) {
	const auto variables = sortedVariables(LogicParser::collectVariables(expr));
	if (variables.size() > kMaxVariables) {
		throw std::invalid_argument("Formula ima previse varijabli (najvise " +
									std::to_string(kMaxVariables) + ")");
	}

	Table table;
	table.variables = variables;
	const std::size_t rowCount = std::size_t{1} << variables.size();
	table.rows.reserve(rowCount);
	for (std::size_t mask = 0; mask < rowCount; ++mask) {
		Row row;
		row.valuation = makeValuation(variables, mask);
		row.result = expr->evaluate(row.valuation);
		table.rows.push_back(std::move(row));
	}
	return table;
}

bool TruthTable::areEquivalent(const LogicExpression::Ptr& lhs, const LogicExpression::Ptr& rhs) {
	std::set<std::string> names = LogicParser::collectVariables(lhs);
	const auto rhsNames = LogicParser::collectVariables(rhs);
	names.insert(rhsNames.begin(), rhsNames.end());
	const auto variables = sortedVariables(names);
	if (variables.size() > kMaxVariables) {
		throw std::invalid_argument("Formule imaju previse varijabli (najvise " +
									std::to_string(kMaxVariables) + ")");
	}

	const std::size_t rowCount = std::size_t{1} << variables.size();
	for (std::size_t mask = 0; mask < rowCount; ++mask) {
		const auto valuation = makeValuation(variables, mask);
		if (lhs->evaluate(valuation) != rhs->evaluate(valuation)) {
			return false;
		}
	}
	return true;
}

} // namespace aksiomat
