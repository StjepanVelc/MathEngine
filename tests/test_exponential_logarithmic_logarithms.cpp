#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/exponential_logarithmic/Logarithms.hpp"

using namespace aksiomat::exponential_logarithmic;

TEST(Logarithms, ComputesBasicLogarithms) {
	EXPECT_DOUBLE_EQ(Logarithms::log(8, 2), 3);
	EXPECT_NEAR(Logarithms::naturalLog(std::exp(1.0)), 1, 1e-12);
	EXPECT_DOUBLE_EQ(Logarithms::commonLog(1000), 3);
	EXPECT_DOUBLE_EQ(Logarithms::binaryLog(8), 3);
}

TEST(Logarithms, ValidatesDomain) {
	EXPECT_THROW(Logarithms::log(0, 2), std::invalid_argument);
	EXPECT_THROW(Logarithms::log(-1, 2), std::invalid_argument);
	EXPECT_THROW(Logarithms::log(8, 1), std::invalid_argument);
	EXPECT_THROW(Logarithms::log(8, -2), std::invalid_argument);
	EXPECT_THROW(Logarithms::naturalLog(0), std::invalid_argument);
}

TEST(Logarithms, ChangesBaseAndAnalyzesFunction) {
	EXPECT_NEAR(Logarithms::changeOfBase(8, 2, 2), 3, 1e-12);
	const auto analysis = Logarithms::analyze({2}, 1, 9, 5);
	EXPECT_EQ(analysis.samples.size(), 5u);
	EXPECT_NEAR(analysis.samples.front().y, 0, 1e-12);
	EXPECT_THROW(Logarithms::analyze({2}, -1, 9, 5), std::invalid_argument);
	EXPECT_THROW(Logarithms::analyze({1}, 1, 9, 5), std::invalid_argument);
}
