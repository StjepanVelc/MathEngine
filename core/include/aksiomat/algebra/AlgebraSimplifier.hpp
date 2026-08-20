#pragma once

#include <string>
#include <vector>

#include "aksiomat/algebra/AlgebraExpression.hpp"

namespace aksiomat::algebra {

struct SimplificationResult {
	AlgebraExpression::Ptr expression;
	std::vector<std::string> steps;
};

class AlgebraSimplifier {
public:
	static SimplificationResult simplify(const AlgebraExpression::Ptr& expression);
};

} // namespace aksiomat::algebra
