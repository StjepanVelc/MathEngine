#pragma once

namespace aksiomat::geometry {

struct Point {
	double x;
	double y;
};

class Coordinates {
public:
	static double distance(Point first, Point second);
	static Point midpoint(Point first, Point second);
};

} // namespace aksiomat::geometry
