#pragma once

namespace aksiomat::analytic_geometry {

struct Point2D {
	double x;
	double y;
};

struct Vector2D {
	double x;
	double y;
};

struct VectorAnalysis {
	Vector2D vector;
	double magnitude;
	double directionDegrees;
};

class PointsVectors {
public:
	static Vector2D fromPoints(Point2D start, Point2D end);
	static Vector2D add(Vector2D left, Vector2D right);
	static Vector2D subtract(Vector2D left, Vector2D right);
	static Vector2D scale(Vector2D vector, double scalar);
	static double dot(Vector2D left, Vector2D right);
	static double determinant(Vector2D left, Vector2D right);
	static double magnitude(Vector2D vector);
	static double angleDegrees(Vector2D left, Vector2D right);
	static VectorAnalysis analyze(Vector2D vector);
};

} // namespace aksiomat::analytic_geometry
