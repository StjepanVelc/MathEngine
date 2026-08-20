#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include "aksiomat/geometry/UnitConversion.hpp"

using namespace aksiomat::geometry;

TEST(GeometryUnitConversion, ConvertsLengthUnits) {
	EXPECT_DOUBLE_EQ(UnitConversion::length(2.5, LengthUnit::Meter, LengthUnit::Centimeter), 250.0);
	EXPECT_DOUBLE_EQ(UnitConversion::length(3500.0, LengthUnit::Meter, LengthUnit::Kilometer), 3.5);
}

TEST(GeometryUnitConversion, ConvertsAreaUnits) {
	EXPECT_DOUBLE_EQ(UnitConversion::area(3.0, AreaUnit::SquareMeter, AreaUnit::SquareCentimeter), 30000.0);
	EXPECT_DOUBLE_EQ(UnitConversion::area(2.0, AreaUnit::Hectare, AreaUnit::SquareMeter), 20000.0);
}

TEST(GeometryUnitConversion, ConvertsVolumeAndCapacityUnits) {
	EXPECT_DOUBLE_EQ(UnitConversion::volume(2.0, VolumeUnit::Liter, VolumeUnit::CubicCentimeter), 2000.0);
	EXPECT_DOUBLE_EQ(UnitConversion::volume(1.5, VolumeUnit::CubicMeter, VolumeUnit::Liter), 1500.0);
	EXPECT_DOUBLE_EQ(UnitConversion::volume(250.0, VolumeUnit::Milliliter, VolumeUnit::CubicCentimeter), 250.0);
}

TEST(GeometryUnitConversion, RejectsInvalidValuesAndUnits) {
	EXPECT_THROW(UnitConversion::length(-1.0, LengthUnit::Meter, LengthUnit::Centimeter), std::invalid_argument);
	EXPECT_THROW(UnitConversion::area(std::numeric_limits<double>::infinity(), AreaUnit::SquareMeter, AreaUnit::Hectare), std::invalid_argument);
	EXPECT_THROW(UnitConversion::volume(1.0, static_cast<VolumeUnit>(99), VolumeUnit::Liter), std::invalid_argument);
}
