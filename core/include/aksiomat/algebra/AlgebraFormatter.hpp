#pragma once

#include <string>

#include "aksiomat/algebra/AlgebraExpression.hpp"

namespace aksiomat::algebra {

class AlgebraFormatter {
public:
	static std::string format(const AlgebraExpression::Ptr& expression);
};

} // namespace aksiomat::algebra
