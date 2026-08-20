#include "aksiomat/arithmetic/Arithmetic.hpp"

#include "aksiomat/arithmetic/ArithmeticEvaluator.hpp"
#include "aksiomat/arithmetic/ArithmeticParser.hpp"

namespace aksiomat {

double Arithmetic::evaluate(const std::string& expression) {
	return ArithmeticEvaluator::evaluate(ArithmeticParser::parse(expression));
}

} // namespace aksiomat
