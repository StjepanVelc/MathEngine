#pragma once

#include <map>
#include <set>
#include <string>

#include "aksiomat/predicate/PredicateExpression.hpp"

namespace aksiomat {

class PredicateParser {
public:
	static PredicateExpression::Ptr parse(const std::string& formula);
	static std::set<std::string> collectFreeVariables(const PredicateExpression::Ptr& expr);
	static std::set<std::string> collectBoundVariables(const PredicateExpression::Ptr& expr);
	static std::map<std::string, std::size_t> collectPredicates(const PredicateExpression::Ptr& expr);
};

} // namespace aksiomat
