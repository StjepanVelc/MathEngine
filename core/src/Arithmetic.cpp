#include "aksiomat/Arithmetic.hpp"

#include "aksiomat/ArithmeticEvaluator.hpp"
#include "aksiomat/ArithmeticParser.hpp"

namespace aksiomat {

double Arithmetic::evaluate(const std::string& expression) {
	return ArithmeticEvaluator::evaluate(ArithmeticParser::parse(expression));
}

} // namespace aksiomat
