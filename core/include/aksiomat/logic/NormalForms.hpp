#pragma once

#include <cstddef>

#include "aksiomat/logic/LogicExpression.hpp"

namespace aksiomat {

class NormalForms {
public:
	static constexpr std::size_t kMaxNodes = 4096;

	static LogicExpression::Ptr toNnf(const LogicExpression::Ptr& expression);
	static LogicExpression::Ptr toCnf(const LogicExpression::Ptr& expression);
	static LogicExpression::Ptr toDnf(const LogicExpression::Ptr& expression);
	static LogicExpression::Ptr toCanonicalCnf(const LogicExpression::Ptr& expression);
	static LogicExpression::Ptr toCanonicalDnf(const LogicExpression::Ptr& expression);
};

} // namespace aksiomat
