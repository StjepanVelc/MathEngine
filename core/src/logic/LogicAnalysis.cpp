#include "aksiomat/logic/LogicAnalysis.hpp"

#include "aksiomat/logic/TruthTable.hpp"

namespace aksiomat {

LogicClassification LogicAnalysis::classify(const LogicExpression::Ptr& expression) {
	const auto table = TruthTable::generate(expression);
	bool anyTrue = false;
	bool anyFalse = false;
	for (const auto& row : table.rows) {
		anyTrue = anyTrue || row.result;
		anyFalse = anyFalse || !row.result;
	}

	LogicClassification classification;
	classification.tautology = !anyFalse;
	classification.contradiction = !anyTrue;
	classification.satisfiable = anyTrue;
	classification.contingent = anyTrue && anyFalse;
	return classification;
}

} // namespace aksiomat
