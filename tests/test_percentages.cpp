#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/Percentages.hpp"

using aksiomat::Percentages;

TEST(Percentages, CalculatesPercentageOfValue) {
	EXPECT_DOUBLE_EQ(Percentages::of(15, 240), 36);
}

TEST(Percentages, IncreasesAndDecreases) {
	EXPECT_DOUBLE_EQ(Percentages::increase(120, 20), 144);
	EXPECT_DOUBLE_EQ(Percentages::decrease(120, 20), 96);
}

TEST(Percentages, CalculatesRatio) {
	EXPECT_DOUBLE_EQ(Percentages::ratio(84, 120), 70);
	EXPECT_THROW(Percentages::ratio(1, 0), std::invalid_argument);
}

TEST(Percentages, RestoresOriginalValues) {
	EXPECT_DOUBLE_EQ(Percentages::originalBeforeIncrease(230, 15), 200);
	EXPECT_DOUBLE_EQ(Percentages::originalBeforeDecrease(80, 20), 100);
	EXPECT_THROW(Percentages::originalBeforeDecrease(1, 100), std::invalid_argument);
}
