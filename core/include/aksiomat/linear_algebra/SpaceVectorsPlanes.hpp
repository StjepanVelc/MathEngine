#pragma once

#include <optional>
#include <string>
#include <vector>

namespace aksiomat::linear_algebra {

struct Vector3D {
	double x;
	double y;
	double z;
};

struct Point3D {
	double x;
	double y;
	double z;
};

struct VectorAnalysisResult {
	Vector3D vector;
	double magnitude;
	std::vector<std::string> steps;
};

struct CrossProductResult {
	Vector3D crossProduct;
	double parallelepipedVolumeHint;
	std::vector<std::string> steps;
};

struct LinePlaneRelation {
	bool lineLiesOnPlane;
	bool lineParallelToPlane;
	bool intersects;
	std::optional<Point3D> intersectionPoint;
	std::vector<std::string> steps;
};

struct PlaneFromPointsResult {
	double normalX;
	double normalY;
	double normalZ;
	double constant;
	std::vector<std::string> steps;
};

class SpaceVectorsPlanes {
public:
	static Vector3D fromPoints(Point3D start, Point3D end);
	static VectorAnalysisResult analyze(Vector3D vector);
	static double dot(Vector3D left, Vector3D right);
	static CrossProductResult cross(Vector3D left, Vector3D right);
	static double distancePointToPlane(Point3D point, double normalX, double normalY, double normalZ, double constant);
	static PlaneFromPointsResult planeFromThreePoints(Point3D first, Point3D second, Point3D third);
	static LinePlaneRelation lineVsPlane(Point3D linePoint, Vector3D lineDirection, double normalX, double normalY, double normalZ, double constant);
};

} // namespace aksiomat::linear_algebra
