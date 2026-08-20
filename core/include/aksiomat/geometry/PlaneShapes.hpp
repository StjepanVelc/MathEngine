#pragma once

namespace aksiomat::geometry {

struct PlaneShapeResult {
	double perimeter;
	double area;
};

class PlaneShapes {
public:
	static PlaneShapeResult square(double side);
	static PlaneShapeResult rectangle(double width, double height);
	static PlaneShapeResult triangle(double sideA, double sideB, double sideC);
	static PlaneShapeResult parallelogram(double base, double side, double height);
	static PlaneShapeResult trapezoid(double baseA, double baseB, double sideC, double sideD, double height);
	static PlaneShapeResult circle(double radius);
};

} // namespace aksiomat::geometry
