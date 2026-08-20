#pragma once

namespace aksiomat::geometry {

struct SolidResult {
	double surfaceArea;
	double volume;
};

class Solids {
public:
	static SolidResult cube(double side);
	static SolidResult cuboid(double length, double width, double height);
	static SolidResult prism(double baseArea, double basePerimeter, double height);
	static SolidResult cylinder(double radius, double height);
};

} // namespace aksiomat::geometry
