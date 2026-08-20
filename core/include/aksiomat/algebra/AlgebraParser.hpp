#pragma once

#include <string>

#include "aksiomat/algebra/AlgebraExpression.hpp"

namespace aksiomat::algebra {

class AlgebraParser {
public:
	static AlgebraExpression::Ptr parse(const std::string& expression);
};

} // namespace aksiomat::algebra
