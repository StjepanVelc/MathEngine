#pragma once

#include <string>
#include <vector>

namespace aksiomat::linear_algebra {

enum class QuadricType {
	Ellipsoid,
	HyperboloidOneSheet,
	HyperboloidTwoSheets,
	EllipticParaboloid,
	HyperbolicParaboloid,
	EllipticCone,
	Unknown
};

struct QuadricClassificationResult {
	QuadricType type;
	std::string typeName;
	std::vector<std::string> steps;
};

class Quadrics {
public:
	// Klasificira opcu kvadriku u kanonskom (centriranom) obliku:
	// A*x^2 + B*y^2 + C*z^2 + D = 0 (elipsoid/hiperboloidi) ili
	// A*x^2 + B*y^2 + E*z = 0 (paraboloidi), ovisno o parametru isParaboloid.
	static QuadricClassificationResult classifyCentral(double a, double b, double c, double d);
	static QuadricClassificationResult classifyParaboloid(double a, double b, double e);
};

} // namespace aksiomat::linear_algebra
