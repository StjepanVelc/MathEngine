#pragma once

#include "aksiomat/logic/LogicExpression.hpp"

namespace aksiomat {

struct LogicClassification {
	bool tautology = false;
	bool contradiction = false;
	bool satisfiable = false;
	bool contingent = false;
};

class LogicAnalysis {
public:
	static LogicClassification classify(const LogicExpression::Ptr& expression);
};

} // namespace aksiomat
