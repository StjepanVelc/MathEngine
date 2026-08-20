#pragma once

#include <string>

#include "aksiomat/arithmetic/ArithmeticExpression.hpp"

namespace aksiomat {

class ArithmeticParser {
public:
	static ArithmeticExpression::Ptr parse(const std::string& expression);
};

} // namespace aksiomat
