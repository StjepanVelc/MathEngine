#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

#include "aksiomat/mathematical_analysis/MultivariableCalculus.hpp"

using aksiomat::mathematical_analysis::MultivariableCalculus;

TEST(MultivariableCalculus, ComputesPartialDerivativesOfProductFunction) {
	// f(x, y) = x^2 * y => df/dx = 2xy, df/dy = x^2
	const auto f = [](double x, double y) { return x * x * y; };
	const auto result = MultivariableCalculus::partialDerivatives(f, "x^2*y", 2.0, 3.0);
	EXPECT_NEAR(result.partialX, 12.0, 1e-3);
	EXPECT_NEAR(result.partialY, 4.0, 1e-3);
	EXPECT_NEAR(result.gradientMagnitude, std::sqrt(12.0 * 12.0 + 4.0 * 4.0), 1e-2);
	EXPECT_FALSE(result.steps.empty());
}

TEST(MultivariableCalculus, RejectsNonFiniteInputs) {
	const auto f = [](double x, double y) { return x + y; };
	EXPECT_THROW(MultivariableCalculus::partialDerivatives(f, "x+y", std::numeric_limits<double>::infinity(), 1.0),
		std::invalid_argument);
}

TEST(MultivariableCalculus, ComputesDirectionalDerivativeAlongGradientDirection) {
	// f(x, y) = x^2 + y^2, gradient at (1,1) = (2,2); directional derivative along (1,1)/sqrt(2) should be sqrt(2)*2 ~ 2.828
	const auto f = [](double x, double y) { return x * x + y * y; };
	const auto result = MultivariableCalculus::directionalDerivative(f, 1.0, 1.0, 1.0, 1.0);
	EXPECT_NEAR(result.directionalDerivative, 2.0 * std::sqrt(2.0), 1e-2);
	EXPECT_FALSE(result.steps.empty());
}

TEST(MultivariableCalculus, RejectsZeroDirectionVector) {
	const auto f = [](double x, double y) { return x + y; };
	EXPECT_THROW(MultivariableCalculus::directionalDerivative(f, 1.0, 1.0, 0.0, 0.0), std::invalid_argument);
}
