#pragma once

namespace aksiomat::geometry {

enum class LengthUnit {
	Millimeter,
	Centimeter,
	Decimeter,
	Meter,
	Kilometer
};

enum class AreaUnit {
	SquareMillimeter,
	SquareCentimeter,
	SquareDecimeter,
	SquareMeter,
	Hectare,
	SquareKilometer
};

enum class VolumeUnit {
	CubicMillimeter,
	CubicCentimeter,
	CubicDecimeter,
	CubicMeter,
	Milliliter,
	Liter
};

class UnitConversion {
public:
	static double length(double value, LengthUnit from, LengthUnit to);
	static double area(double value, AreaUnit from, AreaUnit to);
	static double volume(double value, VolumeUnit from, VolumeUnit to);
};

} // namespace aksiomat::geometry
