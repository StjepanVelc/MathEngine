#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/mathematical_analysis/AdvancedDerivatives.hpp"

using aksiomat::mathematical_analysis::AdvancedDerivatives;

TEST(AdvancedDerivatives, ComputesSecondDerivativeOfPolynomial) {
	const auto result = AdvancedDerivatives::nthDerivative("x^3 + 2x^2 + x", 2, 1.0);
	// f'(x) = 3x^2 + 4x + 1, f''(x) = 6x + 4, f''(1) = 10
	EXPECT_DOUBLE_EQ(result.derivativeValueAtPoint, 10.0);
	EXPECT_EQ(result.order, 2);
	EXPECT_FALSE(result.steps.empty());
}

TEST(AdvancedDerivatives, ComputesThirdDerivativeOfCubic) {
	const auto result = AdvancedDerivatives::nthDerivative("x^3", 3, 5.0);
	// f'''(x) = 6, constant regardless of point
	EXPECT_DOUBLE_EQ(result.derivativeValueAtPoint, 6.0);
}

TEST(AdvancedDerivatives, RejectsNonPositiveOrder) {
	EXPECT_THROW(AdvancedDerivatives::nthDerivative("x^2", 0, 1.0), std::invalid_argument);
}

TEST(AdvancedDerivatives, ComputesChainRuleDerivative) {
	// h(x) = f(g(x)) with f(x) = x^2, g(x) = 2x + 1 => h'(x) = 2*g(x)*2 = 4*(2x+1)
	const auto result = AdvancedDerivatives::chainRule("x^2", "2x + 1", 1.0);
	EXPECT_DOUBLE_EQ(result.innerValueAtPoint, 3.0);
	EXPECT_DOUBLE_EQ(result.outerDerivativeAtInnerValue, 6.0);
	EXPECT_DOUBLE_EQ(result.innerDerivativeAtPoint, 2.0);
	EXPECT_DOUBLE_EQ(result.compositeDerivativeAtPoint, 12.0);
	EXPECT_FALSE(result.steps.empty());
}
