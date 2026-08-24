#pragma once

namespace aksiomat::trigonometry {

enum class AngleUnit {
	Degrees,
	Radians
};

struct UnitCirclePoint {
	double degrees;
	double radians;
	double x;
	double y;
	int quadrant;
	double referenceDegrees;
};

class Angles {
public:
	static double toRadians(double degrees);
	static double toDegrees(double radians);
	static double normalizeDegrees(double degrees);
	static double normalizeRadians(double radians);
	static UnitCirclePoint unitCircle(double angle, AngleUnit unit = AngleUnit::Degrees);
};

} // namespace aksiomat::trigonometry
