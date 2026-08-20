#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/NumeralSystems.hpp"

using aksiomat::NumeralSystems;

TEST(NumeralSystems, ParsesCommonBases) {
	EXPECT_EQ(NumeralSystems::parse("11111111", 2), 255);
	EXPECT_EQ(NumeralSystems::parse("377", 8), 255);
	EXPECT_EQ(NumeralSystems::parse("FF", 16), 255);
	EXPECT_EQ(NumeralSystems::parse("-FF", 16), -255);
}

TEST(NumeralSystems, FormatsCommonBases) {
	EXPECT_EQ(NumeralSystems::format(255, 2), "11111111");
	EXPECT_EQ(NumeralSystems::format(255, 8), "377");
	EXPECT_EQ(NumeralSystems::format(255, 16), "FF");
	EXPECT_EQ(NumeralSystems::format(-255, 16), "-FF");
}

TEST(NumeralSystems, ConvertsBetweenBases) {
	EXPECT_EQ(NumeralSystems::convert("1010", 2, 16), "A");
	EXPECT_EQ(NumeralSystems::convert("7B", 16, 10), "123");
}

TEST(NumeralSystems, HandlesInt64Limits) {
	const auto minimum = std::numeric_limits<std::int64_t>::min();
	EXPECT_EQ(NumeralSystems::parse(NumeralSystems::format(minimum, 16), 16), minimum);
}

TEST(NumeralSystems, RejectsInvalidInput) {
	EXPECT_THROW(NumeralSystems::parse("2", 2), std::invalid_argument);
	EXPECT_THROW(NumeralSystems::parse("", 10), std::invalid_argument);
	EXPECT_THROW(NumeralSystems::format(1, 1), std::invalid_argument);
}
