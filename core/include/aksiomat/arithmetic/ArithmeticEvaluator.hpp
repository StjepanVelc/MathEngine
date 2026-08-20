#pragma once

#include "aksiomat/arithmetic/ArithmeticExpression.hpp"

namespace aksiomat {

class ArithmeticEvaluator {
public:
	static double evaluate(const ArithmeticExpression::Ptr& expression);
};

} // namespace aksiomat
