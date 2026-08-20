#pragma once

#include <string>

#include "aksiomat/algebra/AlgebraExpression.hpp"

namespace aksiomat::algebra {

struct LinearForm {
	double coefficient = 0.0;
	double constant = 0.0;
};

LinearForm extractLinearForm(const AlgebraExpression::Ptr& expression, const std::string& variable = "x");
std::string formatLinearForm(const LinearForm& form, const std::string& variable = "x");

} // namespace aksiomat::algebra
