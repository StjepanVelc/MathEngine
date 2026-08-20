#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/arithmetic/Rational.hpp"

using aksiomat::Rational;

TEST(Rational, NormalizesSignsAndReduces) {
	EXPECT_EQ(Rational(6, 8).toString(), "3/4");
	EXPECT_EQ(Rational(1, -2).toString(), "-1/2");
	EXPECT_EQ(Rational(-2, -4).toString(), "1/2");
}

TEST(Rational, DetectsInt64Overflow) {
	const auto maximum = std::numeric_limits<std::int64_t>::max();
	EXPECT_THROW(Rational(maximum) + Rational(1), std::overflow_error);
	EXPECT_THROW(Rational(maximum) * Rational(2), std::overflow_error);
}

TEST(Rational, PerformsExactOperations) {
	EXPECT_EQ((Rational(1, 2) + Rational(1, 3)).toString(), "5/6");
	EXPECT_EQ((Rational(3, 4) - Rational(1, 2)).toString(), "1/4");
	EXPECT_EQ((Rational(2, 3) * Rational(9, 4)).toString(), "3/2");
	EXPECT_EQ((Rational(2, 3) / Rational(4, 5)).toString(), "5/6");
}

TEST(Rational, ParsesFractionAndDecimal) {
	EXPECT_EQ(Rational::parse("-10/20").toString(), "-1/2");
	EXPECT_EQ(Rational::fromDecimal("1.25").toString(), "5/4");
	EXPECT_EQ(Rational::fromDecimal("-0.125").toString(), "-1/8");
}

TEST(Rational, FormatsMixedAndDecimal) {
	EXPECT_EQ(Rational(7, 3).toMixedString(), "2 1/3");
	EXPECT_DOUBLE_EQ(Rational(1, 4).toDouble(), 0.25);
}

TEST(Rational, RejectsInvalidValues) {
	EXPECT_THROW(Rational(1, 0), std::invalid_argument);
	EXPECT_THROW(Rational::parse("1/2/3"), std::invalid_argument);
	EXPECT_THROW(Rational(1, 2) / Rational(0), std::invalid_argument);
}
