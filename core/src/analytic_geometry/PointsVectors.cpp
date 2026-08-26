#include "aksiomat/analytic_geometry/PointsVectors.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace aksiomat::analytic_geometry {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Coordinates and scalars must be finite");
}

void requireFinite(Point2D point) {
	requireFinite(point.x); requireFinite(point.y);
}

void requireFinite(Vector2D vector) {
	requireFinite(vector.x); requireFinite(vector.y);
}

} // namespace

Vector2D PointsVectors::fromPoints(Point2D start, Point2D end) {
	requireFinite(start); requireFinite(end);
	return {end.x - start.x, end.y - start.y};
}

Vector2D PointsVectors::add(Vector2D left, Vector2D right) {
	requireFinite(left); requireFinite(right);
	return {left.x + right.x, left.y + right.y};
}

Vector2D PointsVectors::subtract(Vector2D left, Vector2D right) {
	requireFinite(left); requireFinite(right);
	return {left.x - right.x, left.y - right.y};
}

Vector2D PointsVectors::scale(Vector2D vector, double scalar) {
	requireFinite(vector); requireFinite(scalar);
	const Vector2D result{vector.x * scalar, vector.y * scalar}; requireFinite(result); return result;
}

double PointsVectors::dot(Vector2D left, Vector2D right) {
	requireFinite(left); requireFinite(right);
	const double result = left.x * right.x + left.y * right.y; requireFinite(result); return result;
}

double PointsVectors::determinant(Vector2D left, Vector2D right) {
	requireFinite(left); requireFinite(right);
	const double result = left.x * right.y - left.y * right.x; requireFinite(result); return result;
}

double PointsVectors::magnitude(Vector2D vector) {
	requireFinite(vector);
	return std::hypot(vector.x, vector.y);
}

double PointsVectors::angleDegrees(Vector2D left, Vector2D right) {
	const double leftMagnitude = magnitude(left), rightMagnitude = magnitude(right);
	if (leftMagnitude == 0.0 || rightMagnitude == 0.0) throw std::invalid_argument("Angle is undefined for a zero vector");
	const double cosine = std::clamp(dot(left, right) / (leftMagnitude * rightMagnitude), -1.0, 1.0);
	return std::acos(cosine) * 180.0 / std::numbers::pi;
}

VectorAnalysis PointsVectors::analyze(Vector2D vector) {
	const double length = magnitude(vector);
	if (length == 0.0) throw std::invalid_argument("Zero vector has no direction angle");
	double direction = std::atan2(vector.y, vector.x) * 180.0 / std::numbers::pi;
	if (direction < 0.0) direction += 360.0;
	return {vector, length, direction};
}

} // namespace aksiomat::analytic_geometry
