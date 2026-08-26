#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/exponential_logarithmic/PowersRoots.hpp"

using namespace aksiomat::exponential_logarithmic;

TEST(PowersRoots, ComputesPowers) {
	EXPECT_DOUBLE_EQ(PowersRoots::power(2, 10).value, 1024);
	EXPECT_DOUBLE_EQ(PowersRoots::power(2, -2).value, 0.25);
	EXPECT_DOUBLE_EQ(PowersRoots::power(-2, 3).value, -8);
	EXPECT_THROW(PowersRoots::power(0, -1), std::invalid_argument);
	EXPECT_THROW(PowersRoots::power(-2, 0.5), std::invalid_argument);
}

TEST(PowersRoots, ComputesRoots) {
	EXPECT_DOUBLE_EQ(PowersRoots::root(9, 2).value, 3);
	EXPECT_NEAR(PowersRoots::root(-8, 3).value, -2, 1e-12);
	EXPECT_THROW(PowersRoots::root(-4, 2), std::invalid_argument);
	EXPECT_THROW(PowersRoots::root(4, 0), std::invalid_argument);
}

TEST(PowersRoots, ComputesRationalPowersAndPerfectSquares) {
	EXPECT_NEAR(PowersRoots::rationalPower(27, 2, 3), 9, 1e-9);
	EXPECT_THROW(PowersRoots::rationalPower(4, 1, 0), std::invalid_argument);
	EXPECT_TRUE(PowersRoots::isPerfectSquare(16));
	EXPECT_FALSE(PowersRoots::isPerfectSquare(15));
	EXPECT_FALSE(PowersRoots::isPerfectSquare(-4));
}
