#include "aksiomat/linear_algebra/SpaceVectorsPlanes.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::linear_algebra {

namespace {

constexpr double maximumMagnitude = 1e6;

void requireFinite(double value, const char* name) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(std::string(name) + " mora biti konacan broj.");
	}
	if (std::abs(value) > maximumMagnitude) {
		throw std::invalid_argument(std::string(name) + " mora biti u rasponu [-1e6, 1e6].");
	}
}

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

} // namespace

Vector3D SpaceVectorsPlanes::fromPoints(Point3D start, Point3D end) {
	requireFinite(start.x, "x1"); requireFinite(start.y, "y1"); requireFinite(start.z, "z1");
	requireFinite(end.x, "x2"); requireFinite(end.y, "y2"); requireFinite(end.z, "z2");
	return Vector3D{end.x - start.x, end.y - start.y, end.z - start.z};
}

VectorAnalysisResult SpaceVectorsPlanes::analyze(Vector3D vector) {
	requireFinite(vector.x, "x"); requireFinite(vector.y, "y"); requireFinite(vector.z, "z");
	const double magnitude = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	std::vector<std::string> steps;
	steps.push_back("Vektor u prostoru: (" + formatNumber(vector.x) + ", " + formatNumber(vector.y) + ", " + formatNumber(vector.z) + ").");
	steps.push_back("Duljina vektora: |v| = sqrt(x^2 + y^2 + z^2) = " + formatNumber(magnitude) + ".");
	return VectorAnalysisResult{vector, magnitude, std::move(steps)};
}

double SpaceVectorsPlanes::dot(Vector3D left, Vector3D right) {
	requireFinite(left.x, "x1"); requireFinite(left.y, "y1"); requireFinite(left.z, "z1");
	requireFinite(right.x, "x2"); requireFinite(right.y, "y2"); requireFinite(right.z, "z2");
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

CrossProductResult SpaceVectorsPlanes::cross(Vector3D left, Vector3D right) {
	requireFinite(left.x, "x1"); requireFinite(left.y, "y1"); requireFinite(left.z, "z1");
	requireFinite(right.x, "x2"); requireFinite(right.y, "y2"); requireFinite(right.z, "z2");
	Vector3D result{
		left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x
	};
	const double magnitude = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z);
	std::vector<std::string> steps;
	steps.push_back("Vektorski produkt a x b racuna se komponentno preko determinante.");
	steps.push_back("Rezultat: (" + formatNumber(result.x) + ", " + formatNumber(result.y) + ", " + formatNumber(result.z) + ").");
	steps.push_back("Duljina vektorskog produkta jednaka je povrsini paralelograma nad a i b: " + formatNumber(magnitude) + ".");
	return CrossProductResult{result, magnitude, std::move(steps)};
}

double SpaceVectorsPlanes::distancePointToPlane(Point3D point, double normalX, double normalY, double normalZ, double constant) {
	requireFinite(point.x, "x"); requireFinite(point.y, "y"); requireFinite(point.z, "z");
	requireFinite(normalX, "A"); requireFinite(normalY, "B"); requireFinite(normalZ, "C"); requireFinite(constant, "D");
	const double normalLength = std::sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);
	if (normalLength == 0.0) {
		throw std::invalid_argument("Normala ravnine ne smije biti nul-vektor.");
	}
	return std::abs(normalX * point.x + normalY * point.y + normalZ * point.z + constant) / normalLength;
}

PlaneFromPointsResult SpaceVectorsPlanes::planeFromThreePoints(Point3D first, Point3D second, Point3D third) {
	requireFinite(first.x, "x1"); requireFinite(first.y, "y1"); requireFinite(first.z, "z1");
	requireFinite(second.x, "x2"); requireFinite(second.y, "y2"); requireFinite(second.z, "z2");
	requireFinite(third.x, "x3"); requireFinite(third.y, "y3"); requireFinite(third.z, "z3");

	const Vector3D u = fromPoints(first, second);
	const Vector3D v = fromPoints(first, third);
	const CrossProductResult normal = cross(u, v);
	if (normal.crossProduct.x == 0.0 && normal.crossProduct.y == 0.0 && normal.crossProduct.z == 0.0) {
		throw std::invalid_argument("Tocke su kolinearne, ne odreduju jedinstvenu ravninu.");
	}
	const double d = -(normal.crossProduct.x * first.x + normal.crossProduct.y * first.y + normal.crossProduct.z * first.z);
	std::vector<std::string> steps;
	steps.push_back("Normala ravnine dobiva se vektorskim produktom vektora AB i AC.");
	steps.push_back("Jednadzba ravnine: " + formatNumber(normal.crossProduct.x) + "x + " + formatNumber(normal.crossProduct.y) + "y + " + formatNumber(normal.crossProduct.z) + "z + " + formatNumber(d) + " = 0.");
	return PlaneFromPointsResult{normal.crossProduct.x, normal.crossProduct.y, normal.crossProduct.z, d, std::move(steps)};
}

LinePlaneRelation SpaceVectorsPlanes::lineVsPlane(Point3D linePoint, Vector3D lineDirection, double normalX, double normalY, double normalZ, double constant) {
	requireFinite(linePoint.x, "x0"); requireFinite(linePoint.y, "y0"); requireFinite(linePoint.z, "z0");
	requireFinite(lineDirection.x, "dx"); requireFinite(lineDirection.y, "dy"); requireFinite(lineDirection.z, "dz");
	requireFinite(normalX, "A"); requireFinite(normalY, "B"); requireFinite(normalZ, "C"); requireFinite(constant, "D");

	const double normalDotDirection = normalX * lineDirection.x + normalY * lineDirection.y + normalZ * lineDirection.z;
	const double pointValue = normalX * linePoint.x + normalY * linePoint.y + normalZ * linePoint.z + constant;

	std::vector<std::string> steps;
	if (std::abs(normalDotDirection) < 1e-9) {
		steps.push_back("Skalarni produkt normale i vektora smjera pravca priblizno je nula, pa je pravac paralelan s ravninom.");
		if (std::abs(pointValue) < 1e-9) {
			steps.push_back("Tocka pravca zadovoljava jednadzbu ravnine, pa pravac lezi na ravnini.");
			return LinePlaneRelation{true, false, false, std::nullopt, std::move(steps)};
		}
		steps.push_back("Tocka pravca ne zadovoljava jednadzbu ravnine, pravac je strogo paralelan s ravninom.");
		return LinePlaneRelation{false, true, false, std::nullopt, std::move(steps)};
	}

	const double t = -pointValue / normalDotDirection;
	Point3D intersection{linePoint.x + t * lineDirection.x, linePoint.y + t * lineDirection.y, linePoint.z + t * lineDirection.z};
	steps.push_back("Uvrstavamo parametarski zapis pravca u jednadzbu ravnine i rjesavamo po parametru t = " + formatNumber(t) + ".");
	steps.push_back("Sjeciste: (" + formatNumber(intersection.x) + ", " + formatNumber(intersection.y) + ", " + formatNumber(intersection.z) + ").");
	return LinePlaneRelation{false, false, true, intersection, std::move(steps)};
}

} // namespace aksiomat::linear_algebra
