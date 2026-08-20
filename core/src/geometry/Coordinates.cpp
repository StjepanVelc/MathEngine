#include "aksiomat/geometry/Coordinates.hpp"

#include <cmath>
#include <numeric>
#include <stdexcept>

namespace aksiomat::geometry {
namespace {

void requireFinite(Point point) {
	if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
		throw std::invalid_argument("Koordinate moraju biti konacni brojevi");
	}
}

} // namespace

double Coordinates::distance(Point first, Point second) {
	requireFinite(first);
	requireFinite(second);
	return std::hypot(second.x - first.x, second.y - first.y);
}

Point Coordinates::midpoint(Point first, Point second) {
	requireFinite(first);
	requireFinite(second);
	return {std::midpoint(first.x, second.x), std::midpoint(first.y, second.y)};
}

} // namespace aksiomat::geometry
