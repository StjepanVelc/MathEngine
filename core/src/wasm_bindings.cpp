// WASM bindings za aksiomat_core — kompajlira se SAMO pod Emscripten toolchainom.
// Native (MSVC) build ovu datoteku ne uključuje (vidi core/CMakeLists.txt).
#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

#include <cmath>
#include <functional>
#include "aksiomat/arithmetic/Arithmetic.hpp"
#include "aksiomat/arithmetic/NumberTheory.hpp"
#include "aksiomat/arithmetic/NumeralSystems.hpp"
#include "aksiomat/arithmetic/Percentages.hpp"
#include "aksiomat/arithmetic/Rational.hpp"
#include "aksiomat/logic/LogicExpression.hpp"
#include "aksiomat/logic/LogicAnalysis.hpp"
#include "aksiomat/logic/LogicParser.hpp"
#include "aksiomat/logic/NormalForms.hpp"
#include "aksiomat/predicate/Interpretation.hpp"
#include "aksiomat/predicate/PredicateParser.hpp"
#include "aksiomat/logic/TruthTable.hpp"
#include "aksiomat/algebra/AlgebraFormatter.hpp"
#include "aksiomat/algebra/AlgebraParser.hpp"
#include "aksiomat/algebra/AlgebraSimplifier.hpp"
#include "aksiomat/algebra/EquationSolver.hpp"
#include "aksiomat/algebra/FunctionAnalyzer.hpp"
#include "aksiomat/algebra/InequalitySolver.hpp"
#include "aksiomat/algebra/LinearSystemSolver.hpp"
#include "aksiomat/algebra/Polynomial.hpp"
#include "aksiomat/geometry/Coordinates.hpp"
#include "aksiomat/geometry/PlaneShapes.hpp"
#include "aksiomat/geometry/Solids.hpp"
#include "aksiomat/geometry/Triangles.hpp"
#include "aksiomat/geometry/UnitConversion.hpp"
#include "aksiomat/trigonometry/Angles.hpp"
#include "aksiomat/trigonometry/GeneralTriangle.hpp"
#include "aksiomat/trigonometry/IdentitiesEquations.hpp"
#include "aksiomat/trigonometry/RightTriangle.hpp"
#include "aksiomat/trigonometry/TrigFunctions.hpp"
#include "aksiomat/sequences/Applications.hpp"
#include "aksiomat/sequences/ArithmeticSequence.hpp"
#include "aksiomat/sequences/GeometricSequence.hpp"
#include "aksiomat/sequences/Recurrences.hpp"
#include "aksiomat/sequences/SequenceAnalysis.hpp"
#include "aksiomat/analytic_geometry/Circles.hpp"
#include "aksiomat/analytic_geometry/Conics.hpp"
#include "aksiomat/analytic_geometry/LinesDistances.hpp"
#include "aksiomat/analytic_geometry/PointsVectors.hpp"
#include "aksiomat/exponential_logarithmic/PowersRoots.hpp"
#include "aksiomat/exponential_logarithmic/ExponentialFunctions.hpp"
#include "aksiomat/exponential_logarithmic/Logarithms.hpp"
#include "aksiomat/exponential_logarithmic/EquationSolvers.hpp"
#include "aksiomat/exponential_logarithmic/Applications.hpp"
#include "aksiomat/combinatorics_probability_statistics/Counting.hpp"
#include "aksiomat/combinatorics_probability_statistics/ProbabilityBasics.hpp"
#include "aksiomat/combinatorics_probability_statistics/DescriptiveStatistics.hpp"
#include "aksiomat/combinatorics_probability_statistics/DataVisualization.hpp"
#include "aksiomat/calculus_basics/Limits.hpp"
#include "aksiomat/calculus_basics/Derivatives.hpp"
#include "aksiomat/calculus_basics/DerivativeApplications.hpp"
#include "aksiomat/calculus_basics/DefiniteIntegral.hpp"
#include "aksiomat/mathematical_analysis/FormalLimits.hpp"
#include "aksiomat/mathematical_analysis/AdvancedDerivatives.hpp"
#include "aksiomat/mathematical_analysis/AdvancedIntegrals.hpp"
#include "aksiomat/mathematical_analysis/FunctionSeries.hpp"
#include "aksiomat/mathematical_analysis/MultivariableCalculus.hpp"
#include "aksiomat/mathematical_analysis/DifferentialEquations.hpp"
#include "aksiomat/linear_algebra/SpaceVectorsPlanes.hpp"
#include "aksiomat/linear_algebra/Quadrics.hpp"
#include "aksiomat/linear_algebra/Matrices.hpp"
#include "aksiomat/linear_algebra/EigenAnalysis.hpp"
#include "aksiomat/linear_algebra/VectorSpaces.hpp"
#include "aksiomat/discrete_math/SetsRelations.hpp"
#include "aksiomat/discrete_math/GraphTheory.hpp"
#include "aksiomat/discrete_math/Recurrences.hpp"
#include "aksiomat/discrete_math/AdvancedCombinatorics.hpp"

namespace {

std::string formatDouble(double value) {
	std::string text = std::to_string(value);
	text.erase(text.find_last_not_of('0') + 1);
	if (!text.empty() && text.back() == '.') text.pop_back();
	return text;
}

std::string jsonString(const std::string& value);
std::string jsonSteps(const std::vector<std::string>& steps);
std::vector<double> parseNumbers(const std::string& text);

aksiomat::trigonometry::AngleUnit trigAngleUnit(const std::string& unit) {
	if (unit == "degrees") return aksiomat::trigonometry::AngleUnit::Degrees;
	if (unit == "radians") return aksiomat::trigonometry::AngleUnit::Radians;
	throw std::invalid_argument("Nepoznata jedinica kuta");
}

aksiomat::trigonometry::TrigFunction trigFunction(const std::string& function) {
	if (function == "sin") return aksiomat::trigonometry::TrigFunction::Sine;
	if (function == "cos") return aksiomat::trigonometry::TrigFunction::Cosine;
	if (function == "tan") return aksiomat::trigonometry::TrigFunction::Tangent;
	throw std::invalid_argument("Nepoznata trigonometrijska funkcija");
}

aksiomat::trigonometry::KnownSide trigSide(const std::string& side) {
	if (side == "opposite") return aksiomat::trigonometry::KnownSide::Opposite;
	if (side == "adjacent") return aksiomat::trigonometry::KnownSide::Adjacent;
	if (side == "hypotenuse") return aksiomat::trigonometry::KnownSide::Hypotenuse;
	throw std::invalid_argument("Nepoznata vrsta stranice");
}

std::string trigonometryAngle(double angle, std::string unit) {
	try {
		const auto point = aksiomat::trigonometry::Angles::unitCircle(angle, trigAngleUnit(unit));
		return "{\"degrees\":" + formatDouble(point.degrees) + ",\"radians\":" + formatDouble(point.radians) +
			",\"x\":" + formatDouble(point.x) + ",\"y\":" + formatDouble(point.y) +
			",\"quadrant\":" + std::to_string(point.quadrant) +
			",\"referenceDegrees\":" + formatDouble(point.referenceDegrees) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string trigonometryFunction(std::string function, double value, std::string unit, bool inverse) {
	try {
		const auto type = trigFunction(function);
		const auto angleUnit = trigAngleUnit(unit);
		if (inverse) {
			return "{\"value\":" + formatDouble(aksiomat::trigonometry::TrigFunctions::inverse(type, value, angleUnit)) + '}';
		}
		const auto result = aksiomat::trigonometry::TrigFunctions::evaluate(type, value, angleUnit);
		return "{\"value\":" + formatDouble(result.value) + ",\"exact\":" + jsonString(result.exact) +
			",\"hasExactForm\":" + (result.hasExactForm ? "true" : "false") + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string rightTriangleJson(const aksiomat::trigonometry::RightTriangleSolution& result) {
	return "{\"opposite\":" + formatDouble(result.opposite) + ",\"adjacent\":" + formatDouble(result.adjacent) +
		",\"hypotenuse\":" + formatDouble(result.hypotenuse) + ",\"angleDegrees\":" + formatDouble(result.angleDegrees) +
		",\"complementaryAngleDegrees\":" + formatDouble(result.complementaryAngleDegrees) +
		",\"steps\":" + jsonSteps(result.steps) + '}';
}

std::string trigonometryRightTriangle(std::string mode, double first, double second, std::string sideType) {
	try {
		if (mode == "legs") return rightTriangleJson(aksiomat::trigonometry::RightTriangle::fromLegs(first, second));
		if (mode == "legHypotenuse") return rightTriangleJson(
			aksiomat::trigonometry::RightTriangle::fromLegAndHypotenuse(first, second, trigSide(sideType)));
		if (mode == "sideAngle") return rightTriangleJson(
			aksiomat::trigonometry::RightTriangle::fromSideAndAngle(first, trigSide(sideType), second));
		throw std::invalid_argument("Nepoznat nacin zadavanja pravokutnog trokuta");
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string generalTriangleJson(const aksiomat::trigonometry::GeneralTriangleSolution& result) {
	return "{\"sideA\":" + formatDouble(result.sideA) + ",\"sideB\":" + formatDouble(result.sideB) +
		",\"sideC\":" + formatDouble(result.sideC) + ",\"angleA\":" + formatDouble(result.angleA) +
		",\"angleB\":" + formatDouble(result.angleB) + ",\"angleC\":" + formatDouble(result.angleC) +
		",\"area\":" + formatDouble(result.area) + ",\"steps\":" + jsonSteps(result.steps) + '}';
}

std::string trigonometryGeneralTriangle(std::string mode, double first, double second, double third) {
	try {
		if (mode == "sss") return generalTriangleJson(aksiomat::trigonometry::GeneralTriangle::fromSides(first, second, third));
		if (mode == "sas") return generalTriangleJson(
			aksiomat::trigonometry::GeneralTriangle::fromTwoSidesAndIncludedAngle(first, second, third));
		if (mode == "aas") return generalTriangleJson(
			aksiomat::trigonometry::GeneralTriangle::fromSideAndTwoAngles(first, second, third));
		throw std::invalid_argument("Nepoznat nacin zadavanja opceg trokuta");
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string trigonometryIdentity(std::string identity, double angleDegrees) {
	try {
		auto type = aksiomat::trigonometry::TrigIdentity::Pythagorean;
		if (identity == "pythagorean") type = aksiomat::trigonometry::TrigIdentity::Pythagorean;
		else if (identity == "tangentRatio") type = aksiomat::trigonometry::TrigIdentity::TangentRatio;
		else if (identity == "doubleSine") type = aksiomat::trigonometry::TrigIdentity::DoubleAngleSine;
		else if (identity == "doubleCosine") type = aksiomat::trigonometry::TrigIdentity::DoubleAngleCosine;
		else throw std::invalid_argument("Nepoznat trigonometrijski identitet");
		const auto result = aksiomat::trigonometry::IdentitiesEquations::check(type, angleDegrees);
		return std::string("{\"holds\":") + (result.holds ? "true" : "false") +
			",\"left\":" + formatDouble(result.left) + ",\"right\":" + formatDouble(result.right) +
			",\"formula\":" + jsonString(result.formula) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string trigonometryEquation(std::string function, double value, double minimum, double maximum) {
	try {
		const auto solutions = aksiomat::trigonometry::IdentitiesEquations::solve(trigFunction(function), value, minimum, maximum);
		std::string json = "{\"solutions\":[";
		for (std::size_t index = 0; index < solutions.size(); ++index) {
			if (index) json += ',';
			json += formatDouble(solutions[index]);
		}
		return json + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string geometryConvert(std::string category, double value, unsigned from, unsigned to) {
	try {
		double result = 0.0;
		if (category == "length") {
			result = aksiomat::geometry::UnitConversion::length(value,
				static_cast<aksiomat::geometry::LengthUnit>(from), static_cast<aksiomat::geometry::LengthUnit>(to));
		} else if (category == "area") {
			result = aksiomat::geometry::UnitConversion::area(value,
				static_cast<aksiomat::geometry::AreaUnit>(from), static_cast<aksiomat::geometry::AreaUnit>(to));
		} else if (category == "volume") {
			result = aksiomat::geometry::UnitConversion::volume(value,
				static_cast<aksiomat::geometry::VolumeUnit>(from), static_cast<aksiomat::geometry::VolumeUnit>(to));
		} else {
			throw std::invalid_argument("Nepoznata vrsta mjere");
		}
		return "{\"value\":" + formatDouble(result) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string geometryPlaneShape(std::string shape, double a, double b, double c, double d, double height) {
	try {
		auto result = aksiomat::geometry::PlaneShapeResult{};
		if (shape == "square") result = aksiomat::geometry::PlaneShapes::square(a);
		else if (shape == "rectangle") result = aksiomat::geometry::PlaneShapes::rectangle(a, b);
		else if (shape == "triangle") result = aksiomat::geometry::PlaneShapes::triangle(a, b, c);
		else if (shape == "parallelogram") result = aksiomat::geometry::PlaneShapes::parallelogram(a, b, height);
		else if (shape == "trapezoid") result = aksiomat::geometry::PlaneShapes::trapezoid(a, b, c, d, height);
		else if (shape == "circle") result = aksiomat::geometry::PlaneShapes::circle(a);
		else throw std::invalid_argument("Nepoznat ravninski lik");
		return "{\"perimeter\":" + formatDouble(result.perimeter) +
			",\"area\":" + formatDouble(result.area) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string geometryTriangle(std::string operation, double a, double b, double c) {
	try {
		if (operation == "classify") {
			const auto result = aksiomat::geometry::Triangles::classify(a, b, c);
			return "{\"bySides\":" + jsonString(result.bySides) +
				",\"byAngles\":" + jsonString(result.byAngles) + '}';
		}
		double value = 0.0;
		if (operation == "thirdAngle") value = aksiomat::geometry::Triangles::thirdAngle(a, b);
		else if (operation == "hypotenuse") value = aksiomat::geometry::Triangles::hypotenuse(a, b);
		else if (operation == "leg") value = aksiomat::geometry::Triangles::leg(a, b);
		else throw std::invalid_argument("Nepoznata operacija s trokutom");
		return "{\"value\":" + formatDouble(value) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string geometrySolid(std::string solid, double a, double b, double c) {
	try {
		auto result = aksiomat::geometry::SolidResult{};
		if (solid == "cube") result = aksiomat::geometry::Solids::cube(a);
		else if (solid == "cuboid") result = aksiomat::geometry::Solids::cuboid(a, b, c);
		else if (solid == "prism") result = aksiomat::geometry::Solids::prism(a, b, c);
		else if (solid == "cylinder") result = aksiomat::geometry::Solids::cylinder(a, b);
		else throw std::invalid_argument("Nepoznato geometrijsko tijelo");
		return "{\"surfaceArea\":" + formatDouble(result.surfaceArea) +
			",\"volume\":" + formatDouble(result.volume) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string geometryCoordinates(std::string operation, double x1, double y1, double x2, double y2) {
	try {
		const aksiomat::geometry::Point first{x1, y1};
		const aksiomat::geometry::Point second{x2, y2};
		if (operation == "distance") {
			return "{\"distance\":" + formatDouble(aksiomat::geometry::Coordinates::distance(first, second)) + '}';
		}
		if (operation == "midpoint") {
			const auto result = aksiomat::geometry::Coordinates::midpoint(first, second);
			return "{\"x\":" + formatDouble(result.x) + ",\"y\":" + formatDouble(result.y) + '}';
		}
		throw std::invalid_argument("Nepoznata koordinatna operacija");
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string jsonString(const std::string& value) {
	std::string result = "\"";
	for (const char character : value) {
		if (character == '"' || character == '\\') result += '\\';
		if (character == '\n') result += "\\n";
		else if (character != '\r') result += character;
	}
	return result + '"';
}

std::string jsonSteps(const std::vector<std::string>& steps) {
	std::string result = "[";
	for (std::size_t index = 0; index < steps.size(); ++index) {
		if (index) result += ',';
		result += jsonString(steps[index]);
	}
	return result + ']';
}

std::string analyticPointJson(aksiomat::analytic_geometry::Point2D point) {
	return "{\"x\":" + formatDouble(point.x) + ",\"y\":" + formatDouble(point.y) + '}';
}

std::string analyticPointsJson(const std::vector<aksiomat::analytic_geometry::Point2D>& points) {
	std::string result = "[";
	for (std::size_t index = 0; index < points.size(); ++index) {
		if (index) result += ',';
		result += analyticPointJson(points[index]);
	}
	return result + ']';
}

std::string analyticGeometryVectors(double ax, double ay, double bx, double by) {
	try {
		using namespace aksiomat::analytic_geometry;
		const Vector2D first{ax, ay}, second{bx, by};
		const auto sum = PointsVectors::add(first, second);
		return "{\"firstMagnitude\":" + formatDouble(PointsVectors::magnitude(first)) +
			",\"secondMagnitude\":" + formatDouble(PointsVectors::magnitude(second)) +
			",\"dot\":" + formatDouble(PointsVectors::dot(first, second)) +
			",\"determinant\":" + formatDouble(PointsVectors::determinant(first, second)) +
			",\"angleDegrees\":" + formatDouble(PointsVectors::angleDegrees(first, second)) +
			",\"sum\":" + analyticPointJson({sum.x, sum.y}) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string analyticGeometryLines(double x1, double y1, double x2, double y2,
	double x3, double y3, double x4, double y4, double px, double py) {
	try {
		using namespace aksiomat::analytic_geometry;
		const auto first = LinesDistances::throughPoints({x1, y1}, {x2, y2});
		const auto second = LinesDistances::throughPoints({x3, y3}, {x4, y4});
		const auto firstAnalysis = LinesDistances::analyze(first);
		const auto relation = LinesDistances::relation(first, second);
		const char* relationName = relation.relation == LineRelation::Parallel ? "parallel" :
			relation.relation == LineRelation::Coincident ? "coincident" :
			relation.relation == LineRelation::Perpendicular ? "perpendicular" : "intersecting";
		return "{\"first\":{\"a\":" + formatDouble(first.a) + ",\"b\":" + formatDouble(first.b) + ",\"c\":" + formatDouble(first.c) +
			",\"vertical\":" + (firstAnalysis.vertical ? "true" : "false") + ",\"slope\":" + (firstAnalysis.slope ? formatDouble(*firstAnalysis.slope) : "null") +
			"},\"second\":{\"a\":" + formatDouble(second.a) + ",\"b\":" + formatDouble(second.b) + ",\"c\":" + formatDouble(second.c) +
			"},\"relation\":\"" + relationName + "\",\"angleDegrees\":" + formatDouble(relation.acuteAngleDegrees) +
			",\"intersection\":" + (relation.intersection ? analyticPointJson(*relation.intersection) : "null") +
			",\"pointDistance\":" + formatDouble(LinesDistances::pointToLineDistance({px, py}, first)) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string analyticGeometryCircle(std::string mode, double first, double second, double third,
	double fourth, double fifth, double sixth, double px, double py) {
	try {
		using namespace aksiomat::analytic_geometry;
		Circle circle;
		if (mode == "centerRadius") circle = Circles::fromCenterRadius({first, second}, third);
		else if (mode == "general") circle = Circles::fromGeneralEquation(first, second, third);
		else if (mode == "threePoints") circle = Circles::throughThreePoints({first, second}, {third, fourth}, {fifth, sixth});
		else throw std::invalid_argument("Nepoznat nacin zadavanja kruznice");
		const auto result = Circles::analyze(circle);
		const auto position = Circles::classify({px, py}, circle);
		const char* positionName = position == PointCirclePosition::Inside ? "inside" : position == PointCirclePosition::On ? "on" : "outside";
		return "{\"center\":" + analyticPointJson(circle.center) + ",\"radius\":" + formatDouble(circle.radius) +
			",\"circumference\":" + formatDouble(result.circumference) + ",\"area\":" + formatDouble(result.area) +
			",\"pointPosition\":\"" + positionName + "\",\"samples\":" + analyticPointsJson(result.samples) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string analyticGeometryConic(std::string type, double centerX, double centerY,
	double first, double second, bool horizontal) {
	try {
		using namespace aksiomat::analytic_geometry;
		if (type == "parabola") {
			const auto result = Conics::analyze(Parabola{{centerX, centerY}, first,
				horizontal ? ParabolaOrientation::Horizontal : ParabolaOrientation::Vertical});
			return "{\"type\":\"parabola\",\"focus\":" + analyticPointJson(result.focus) +
				",\"directrix\":" + formatDouble(result.directrix) + ",\"samples\":" + analyticPointsJson(result.samples) + '}';
		}
		if (type == "ellipse") {
			const auto result = Conics::analyze(Ellipse{{centerX, centerY}, first, second, horizontal});
			return "{\"type\":\"ellipse\",\"focalDistance\":" + formatDouble(result.focalDistance) +
				",\"eccentricity\":" + formatDouble(result.eccentricity) + ",\"focus1\":" + analyticPointJson(result.focus1) +
				",\"focus2\":" + analyticPointJson(result.focus2) + ",\"samples\":" + analyticPointsJson(result.samples) + '}';
		}
		if (type == "hyperbola") {
			const auto result = Conics::analyze(Hyperbola{{centerX, centerY}, first, second, horizontal});
			return "{\"type\":\"hyperbola\",\"focalDistance\":" + formatDouble(result.focalDistance) +
				",\"eccentricity\":" + formatDouble(result.eccentricity) + ",\"focus1\":" + analyticPointJson(result.focus1) +
				",\"focus2\":" + analyticPointJson(result.focus2) + ",\"branch1\":" + analyticPointsJson(result.branch1) +
				",\"branch2\":" + analyticPointsJson(result.branch2) + '}';
		}
		throw std::invalid_argument("Nepoznata konika");
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string exponentialLogPowersRoots(std::string mode, double base, double exponent, double degree) {
	try {
		using namespace aksiomat::exponential_logarithmic;
		if (mode == "power") {
			const auto result = PowersRoots::power(base, exponent);
			return "{\"mode\":\"power\",\"value\":" + formatDouble(result.value) + '}';
		}
		if (mode == "root") {
			const auto result = PowersRoots::root(base, degree);
			return "{\"mode\":\"root\",\"value\":" + formatDouble(result.value) + '}';
		}
		throw std::invalid_argument("Nepoznat nacin racunanja potencije/korijena");
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string exponentialLogFunction(double initialValue, double base, double xMin, double xMax, int sampleCount) {
	try {
		using namespace aksiomat::exponential_logarithmic;
		const auto analysis = ExponentialFunctions::analyze({initialValue, base}, xMin, xMax, static_cast<std::size_t>(sampleCount));
		std::string samples = "[";
		for (std::size_t index = 0; index < analysis.samples.size(); ++index) {
			if (index) samples += ',';
			samples += "{\"x\":" + formatDouble(analysis.samples[index].x) + ",\"y\":" + formatDouble(analysis.samples[index].y) + '}';
		}
		samples += ']';
		return "{\"isGrowth\":" + std::string(analysis.isGrowth ? "true" : "false") +
			",\"growthRatePercent\":" + formatDouble(analysis.growthRatePercent) +
			",\"samples\":" + samples + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string exponentialLogLogarithm(double base, double xMin, double xMax, int sampleCount) {
	try {
		using namespace aksiomat::exponential_logarithmic;
		const auto analysis = Logarithms::analyze({base}, xMin, xMax, static_cast<std::size_t>(sampleCount));
		std::string samples = "[";
		for (std::size_t index = 0; index < analysis.samples.size(); ++index) {
			if (index) samples += ',';
			samples += "{\"x\":" + formatDouble(analysis.samples[index].x) + ",\"y\":" + formatDouble(analysis.samples[index].y) + '}';
		}
		samples += ']';
		return "{\"samples\":" + samples + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string exponentialLogEquation(std::string kind, double base, double coefficient, double target) {
	try {
		using namespace aksiomat::exponential_logarithmic;
		const auto solution = kind == "exponential"
			? EquationSolvers::solveExponential(base, coefficient, target)
			: EquationSolvers::solveLogarithmic(base, coefficient, target);
		return "{\"solution\":" + formatDouble(solution.solution) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string exponentialLogApplication(std::string kind, double first, double second, double third) {
	try {
		using namespace aksiomat::exponential_logarithmic;
		if (kind == "radioactiveDecay") return "{\"value\":" + formatDouble(Applications::radioactiveDecay(first, second, third).value) + '}';
		if (kind == "phLevel") return "{\"value\":" + formatDouble(Applications::phLevel(first).value) + '}';
		if (kind == "richterMagnitude") return "{\"value\":" + formatDouble(Applications::richterMagnitude(first).value) + '}';
		if (kind == "soundIntensityDecibels") return "{\"value\":" + formatDouble(Applications::soundIntensityDecibels(first, second).value) + '}';
		throw std::invalid_argument("Nepoznata primjena");
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string jsonNumbers(const std::vector<double>& values) {
	std::string result = "[";
	for (std::size_t index = 0; index < values.size(); ++index) {
		if (index) result += ',';
		result += formatDouble(values[index]);
	}
	return result + ']';
}

std::vector<double> splitNumbers(const std::string& text) {
	std::vector<double> values;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(',', position);
		const std::string token = text.substr(position, end == std::string::npos ? end : end - position);
		if (token.empty()) throw std::invalid_argument("Popis podataka sadrzi praznu vrijednost");
		std::size_t parsed = 0;
		const double value = std::stod(token, &parsed);
		if (parsed != token.size()) throw std::invalid_argument("Neispravan podatak");
		values.push_back(value);
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return values;
}

std::string combinatoricsCounting(std::string mode, long long n, long long k) {
	try {
		using namespace aksiomat::combinatorics_probability_statistics;
		double value = 0.0;
		if (mode == "factorial") value = Counting::factorial(n);
		else if (mode == "permutations") value = Counting::permutations(n, k);
		else if (mode == "permutationsWithRepetition") value = Counting::permutationsWithRepetition(n, k);
		else if (mode == "combinations") value = Counting::combinations(n, k);
		else if (mode == "combinationsWithRepetition") value = Counting::combinationsWithRepetition(n, k);
		else throw std::invalid_argument("Nepoznat nacin prebrojavanja");
		return "{\"value\":" + formatDouble(value) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string combinatoricsProbability(std::string mode, double first, double second, double third) {
	try {
		using namespace aksiomat::combinatorics_probability_statistics;
		double value = 0.0;
		if (mode == "classical") value = ProbabilityBasics::classicalProbability(first, second);
		else if (mode == "complement") value = ProbabilityBasics::complementProbability(first);
		else if (mode == "union") value = ProbabilityBasics::unionProbability(first, second, third);
		else if (mode == "conditional") value = ProbabilityBasics::conditionalProbability({first, second});
		else if (mode == "independent") value = ProbabilityBasics::independentEventsProbability(first, second);
		else throw std::invalid_argument("Nepoznat nacin racunanja vjerojatnosti");
		return "{\"value\":" + formatDouble(value) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string combinatoricsStatistics(std::string dataset) {
	try {
		using namespace aksiomat::combinatorics_probability_statistics;
		const auto values = splitNumbers(dataset);
		const double meanValue = DescriptiveStatistics::mean(values);
		const double medianValue = DescriptiveStatistics::median(values);
		const auto modeValues = DescriptiveStatistics::mode(values);
		const double varianceValue = DescriptiveStatistics::variance(values, false);
		const double standardDeviationValue = DescriptiveStatistics::standardDeviation(values, false);
		const double rangeValue = DescriptiveStatistics::range(values);
		std::string result = "{\"mean\":" + formatDouble(meanValue) +
			",\"median\":" + formatDouble(medianValue) +
			",\"mode\":" + jsonNumbers(modeValues) +
			",\"variance\":" + formatDouble(varianceValue) +
			",\"standardDeviation\":" + formatDouble(standardDeviationValue) +
			",\"range\":" + formatDouble(rangeValue);
		if (values.size() >= 2) {
			const auto quartileValues = DescriptiveStatistics::quartiles(values);
			result += ",\"q1\":" + formatDouble(quartileValues.q1) +
				",\"q2\":" + formatDouble(quartileValues.q2) +
				",\"q3\":" + formatDouble(quartileValues.q3) +
				",\"interquartileRange\":" + formatDouble(quartileValues.interquartileRange);
		}
		return result + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string combinatoricsVisualization(std::string dataset, int binCount) {
	try {
		using namespace aksiomat::combinatorics_probability_statistics;
		const auto values = splitNumbers(dataset);
		const auto bins = DataVisualization::buildFrequencyTable(values, binCount);
		std::string binsJson = "[";
		for (std::size_t index = 0; index < bins.size(); ++index) {
			if (index) binsJson += ',';
			binsJson += "{\"lowerBound\":" + formatDouble(bins[index].lowerBound) +
				",\"upperBound\":" + formatDouble(bins[index].upperBound) +
				",\"count\":" + std::to_string(bins[index].count) + '}';
		}
		binsJson += ']';
		return "{\"bins\":" + binsJson + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string calculusLimit(std::string expression, double point, std::string variable) {
	try {
		using namespace aksiomat::calculus_basics;
		const auto result = Limits::evaluate(expression, point, variable);
		std::string leftJson = "[";
		for (std::size_t index = 0; index < result.leftSamples.size(); ++index) {
			if (index) leftJson += ',';
			leftJson += "{\"x\":" + formatDouble(result.leftSamples[index].x) +
				",\"value\":" + formatDouble(result.leftSamples[index].value) + '}';
		}
		leftJson += ']';
		std::string rightJson = "[";
		for (std::size_t index = 0; index < result.rightSamples.size(); ++index) {
			if (index) rightJson += ',';
			rightJson += "{\"x\":" + formatDouble(result.rightSamples[index].x) +
				",\"value\":" + formatDouble(result.rightSamples[index].value) + '}';
		}
		rightJson += ']';
		return "{\"expression\":" + jsonString(result.expression) +
			",\"point\":" + formatDouble(result.point) +
			",\"limitValue\":" + formatDouble(result.limitValue) +
			",\"existsFinite\":" + std::string(result.existsFinite ? "true" : "false") +
			",\"leftSamples\":" + leftJson +
			",\"rightSamples\":" + rightJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string calculusDerivative(std::string expression, double point, std::string variable) {
	try {
		using namespace aksiomat::calculus_basics;
		const auto result = Derivatives::differentiate(expression, point, variable);
		return "{\"original\":" + jsonString(result.original) +
			",\"derivative\":" + jsonString(result.derivative) +
			",\"pointValue\":" + formatDouble(result.pointValue) +
			",\"slopeAtPoint\":" + formatDouble(result.slopeAtPoint) +
			",\"tangentLine\":" + jsonString(result.tangentLine) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string calculusRateOfChange(std::string expression, double a, double b, std::string variable) {
	try {
		using namespace aksiomat::calculus_basics;
		const auto result = Derivatives::rateOfChange(expression, a, b, variable);
		return "{\"averageRate\":" + formatDouble(result.averageRate) +
			",\"instantaneousRate\":" + formatDouble(result.instantaneousRate) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string calculusDerivativeApplications(std::string expression, std::string variable) {
	try {
		using namespace aksiomat::calculus_basics;
		const auto result = DerivativeApplications::analyze(expression, variable);
		std::string criticalJson = "[";
		for (std::size_t index = 0; index < result.criticalPoints.size(); ++index) {
			if (index) criticalJson += ',';
			criticalJson += "{\"x\":" + formatDouble(result.criticalPoints[index].x) +
				",\"y\":" + formatDouble(result.criticalPoints[index].y) +
				",\"kind\":" + jsonString(result.criticalPoints[index].kind) + '}';
		}
		criticalJson += ']';
		std::string increasingJson = "[";
		for (std::size_t index = 0; index < result.increasingIntervals.size(); ++index) {
			if (index) increasingJson += ',';
			increasingJson += jsonString(result.increasingIntervals[index]);
		}
		increasingJson += ']';
		std::string decreasingJson = "[";
		for (std::size_t index = 0; index < result.decreasingIntervals.size(); ++index) {
			if (index) decreasingJson += ',';
			decreasingJson += jsonString(result.decreasingIntervals[index]);
		}
		decreasingJson += ']';
		return "{\"expression\":" + jsonString(result.expression) +
			",\"derivative\":" + jsonString(result.derivative) +
			",\"criticalPoints\":" + criticalJson +
			",\"increasingIntervals\":" + increasingJson +
			",\"decreasingIntervals\":" + decreasingJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string calculusDefiniteIntegral(std::string expression, double lowerBound, double upperBound, std::string variable) {
	try {
		using namespace aksiomat::calculus_basics;
		const auto result = DefiniteIntegral::evaluate(expression, lowerBound, upperBound, variable);
		return "{\"expression\":" + jsonString(result.expression) +
			",\"antiderivative\":" + jsonString(result.antiderivative) +
			",\"lowerBound\":" + formatDouble(result.lowerBound) +
			",\"upperBound\":" + formatDouble(result.upperBound) +
			",\"area\":" + formatDouble(result.area) +
			",\"numericCheck\":" + formatDouble(result.numericCheck) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

// Katalog imenovanih funkcija jedne varijable koje web frontend moze birati iz padajuceg izbornika
// (WASM/JS ne moze proslijediti proizvoljan C++ callable, pa se koristi imenovani katalog).
std::function<double(double)> namedSingleVariableFunction(const std::string& name) {
	if (name == "exp") return [](double x) { return std::exp(x); };
	if (name == "reciprocal_square") return [](double x) { return 1.0 / (x * x + 1.0); };
	if (name == "sin") return [](double x) { return std::sin(x); };
	if (name == "gaussian") return [](double x) { return std::exp(-x * x); };
	throw std::invalid_argument("Nepoznata funkcija za nepravi integral");
}

std::function<double(int, double)> namedDerivativeFunction(const std::string& name) {
	if (name == "exp") return [](int, double x) { return std::exp(x); };
	if (name == "sin") return [](int n, double x) {
		switch (n % 4) {
			case 0: return std::sin(x);
			case 1: return std::cos(x);
			case 2: return -std::sin(x);
			default: return -std::cos(x);
		}
	};
	throw std::invalid_argument("Nepoznata funkcija za Taylorov red");
}

std::function<double(double, double)> namedTwoVariableFunction(const std::string& name) {
	if (name == "x2y") return [](double x, double y) { return x * x * y; };
	if (name == "sum_squares") return [](double x, double y) { return x * x + y * y; };
	if (name == "product") return [](double x, double y) { return x * y; };
	throw std::invalid_argument("Nepoznata funkcija dvije varijable");
}

std::function<double(double, double)> namedOdeFunction(const std::string& name) {
	if (name == "exponential_growth") return [](double, double y) { return y; };
	if (name == "logistic") return [](double, double y) { return y * (1.0 - y); };
	if (name == "harmonic_velocity") return [](double t, double) { return std::cos(t); };
	throw std::invalid_argument("Nepoznata diferencijalna jednadzba");
}

std::string maFormalLimit(std::string expression, double point, std::string variable) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto result = FormalLimits::evaluate(expression, point, variable);
		std::string tableJson = "[";
		for (std::size_t index = 0; index < result.epsilonDeltaTable.size(); ++index) {
			if (index) tableJson += ',';
			tableJson += "{\"epsilon\":" + formatDouble(result.epsilonDeltaTable[index].epsilon) +
				",\"delta\":" + formatDouble(result.epsilonDeltaTable[index].delta) + '}';
		}
		tableJson += ']';
		return "{\"expression\":" + jsonString(result.expression) +
			",\"point\":" + formatDouble(result.point) +
			",\"limitValue\":" + formatDouble(result.limitValue) +
			",\"existsFinite\":" + std::string(result.existsFinite ? "true" : "false") +
			",\"epsilonDeltaTable\":" + tableJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maContinuity(std::string expression, double point, std::string variable) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto result = FormalLimits::checkContinuity(expression, point, variable);
		return "{\"expression\":" + jsonString(result.expression) +
			",\"point\":" + formatDouble(result.point) +
			",\"functionDefinedAtPoint\":" + std::string(result.functionDefinedAtPoint ? "true" : "false") +
			",\"functionValueAtPoint\":" + formatDouble(result.functionValueAtPoint) +
			",\"limitValue\":" + formatDouble(result.limitValue) +
			",\"isContinuous\":" + std::string(result.isContinuous ? "true" : "false") +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maNthDerivative(std::string expression, int order, double point, std::string variable) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto result = AdvancedDerivatives::nthDerivative(expression, order, point, variable);
		return "{\"expression\":" + jsonString(result.expression) +
			",\"order\":" + std::to_string(result.order) +
			",\"derivativeExpression\":" + jsonString(result.derivativeExpression) +
			",\"point\":" + formatDouble(result.point) +
			",\"derivativeValueAtPoint\":" + formatDouble(result.derivativeValueAtPoint) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maChainRule(std::string outerExpression, std::string innerExpression, double point, std::string variable) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto result = AdvancedDerivatives::chainRule(outerExpression, innerExpression, point, variable);
		return "{\"outerExpression\":" + jsonString(result.outerExpression) +
			",\"innerExpression\":" + jsonString(result.innerExpression) +
			",\"point\":" + formatDouble(result.point) +
			",\"innerValueAtPoint\":" + formatDouble(result.innerValueAtPoint) +
			",\"outerDerivativeAtInnerValue\":" + formatDouble(result.outerDerivativeAtInnerValue) +
			",\"innerDerivativeAtPoint\":" + formatDouble(result.innerDerivativeAtPoint) +
			",\"compositeDerivativeAtPoint\":" + formatDouble(result.compositeDerivativeAtPoint) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maImproperIntegral(std::string functionName, double lowerBound) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto integrand = namedSingleVariableFunction(functionName);
		const auto result = AdvancedIntegrals::improperIntegral(integrand, functionName, lowerBound);
		return "{\"description\":" + jsonString(result.description) +
			",\"lowerBound\":" + formatDouble(result.lowerBound) +
			",\"convergent\":" + std::string(result.convergent ? "true" : "false") +
			",\"approximateValue\":" + formatDouble(result.approximateValue) +
			",\"partialSums\":" + jsonNumbers(result.partialSums) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maSubstitutionIntegral(double a, double b, int power, double lowerBound, double upperBound) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto result = AdvancedIntegrals::integrateBySubstitution(a, b, power, lowerBound, upperBound);
		return "{\"linearCoefficientA\":" + formatDouble(result.linearCoefficientA) +
			",\"linearCoefficientB\":" + formatDouble(result.linearCoefficientB) +
			",\"power\":" + std::to_string(result.power) +
			",\"lowerBound\":" + formatDouble(result.lowerBound) +
			",\"upperBound\":" + formatDouble(result.upperBound) +
			",\"value\":" + formatDouble(result.value) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maTaylorSeries(std::string functionName, double center, int order, double point) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto derivativeAt = namedDerivativeFunction(functionName);
		const auto actual = namedSingleVariableFunction(functionName);
		const auto result = FunctionSeries::buildTaylorSeries(derivativeAt, actual, functionName, center, order, point);
		return "{\"description\":" + jsonString(result.description) +
			",\"center\":" + formatDouble(result.center) +
			",\"order\":" + std::to_string(result.order) +
			",\"coefficients\":" + jsonNumbers(result.coefficients) +
			",\"point\":" + formatDouble(result.point) +
			",\"taylorApproximationAtPoint\":" + formatDouble(result.taylorApproximationAtPoint) +
			",\"actualValueAtPoint\":" + formatDouble(result.actualValueAtPoint) +
			",\"approximationError\":" + formatDouble(result.approximationError) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maPowerSeries(std::string coefficientsText, double testPoint) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto coefficients = parseNumbers(coefficientsText);
		const auto result = FunctionSeries::analyzePowerSeries(coefficients, testPoint);
		return "{\"coefficients\":" + jsonNumbers(result.coefficients) +
			",\"radiusOfConvergence\":" + formatDouble(result.radiusOfConvergence) +
			",\"convergesAtPoint\":" + std::string(result.convergesAtPoint ? "true" : "false") +
			",\"testPoint\":" + formatDouble(result.testPoint) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maPartialDerivatives(std::string functionName, double x, double y) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto f = namedTwoVariableFunction(functionName);
		const auto result = MultivariableCalculus::partialDerivatives(f, functionName, x, y);
		return "{\"description\":" + jsonString(result.description) +
			",\"x\":" + formatDouble(result.x) +
			",\"y\":" + formatDouble(result.y) +
			",\"partialX\":" + formatDouble(result.partialX) +
			",\"partialY\":" + formatDouble(result.partialY) +
			",\"gradient\":" + jsonNumbers(result.gradient) +
			",\"gradientMagnitude\":" + formatDouble(result.gradientMagnitude) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string maDirectionalDerivative(std::string functionName, double x, double y, double directionX, double directionY) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto f = namedTwoVariableFunction(functionName);
		const auto result = MultivariableCalculus::directionalDerivative(f, x, y, directionX, directionY);
		return "{\"partialX\":" + formatDouble(result.partialX) +
			",\"partialY\":" + formatDouble(result.partialY) +
			",\"directionX\":" + formatDouble(result.directionX) +
			",\"directionY\":" + formatDouble(result.directionY) +
			",\"directionalDerivative\":" + formatDouble(result.directionalDerivative) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string jsonOdeResult(const aksiomat::mathematical_analysis::OdeSolutionResult& result) {
	std::string pointsJson = "[";
	for (std::size_t index = 0; index < result.points.size(); ++index) {
		if (index) pointsJson += ',';
		pointsJson += "{\"t\":" + formatDouble(result.points[index].t) +
			",\"y\":" + formatDouble(result.points[index].y) + '}';
	}
	pointsJson += ']';
	return "{\"description\":" + jsonString(result.description) +
		",\"method\":" + jsonString(result.method) +
		",\"initialT\":" + formatDouble(result.initialT) +
		",\"initialY\":" + formatDouble(result.initialY) +
		",\"stepSize\":" + formatDouble(result.stepSize) +
		",\"points\":" + pointsJson +
		",\"finalValue\":" + formatDouble(result.finalValue) +
		",\"steps\":" + jsonSteps(result.steps) + '}';
}

std::string maSolveOde(std::string functionName, std::string method, double initialT, double initialY, double finalT, double stepSize) {
	try {
		using namespace aksiomat::mathematical_analysis;
		const auto f = namedOdeFunction(functionName);
		if (method == "euler") return jsonOdeResult(DifferentialEquations::solveEuler(f, functionName, initialT, initialY, finalT, stepSize));
		if (method == "rk4") return jsonOdeResult(DifferentialEquations::solveRungeKutta4(f, functionName, initialT, initialY, finalT, stepSize));
		throw std::invalid_argument("Nepoznata numericka metoda za ODE");
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string jsonPartialSums(const std::vector<double>& values) {
	std::vector<double> sums;
	sums.reserve(values.size());
	double sum = 0.0;
	for (const double value : values) {
		sum += value;
		sums.push_back(sum);
	}
	return jsonNumbers(sums);
}

std::vector<double> parseNumbers(const std::string& text) {
	std::vector<double> values;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(',', position);
		const std::string token = text.substr(position, end == std::string::npos ? end : end - position);
		if (token.empty()) throw std::invalid_argument("Popis clanova sadrzi praznu vrijednost");
		std::size_t parsed = 0;
		const double value = std::stod(token, &parsed);
		if (parsed != token.size()) throw std::invalid_argument("Neispravan clan niza");
		values.push_back(value);
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return values;
}

// Redci matrice/vektora odvojeni su znakom ';', a vrijednosti unutar retka znakom ','.
std::vector<std::vector<double>> parseRows(const std::string& text) {
	std::vector<std::vector<double>> rows;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(';', position);
		const std::string row = text.substr(position, end == std::string::npos ? end : end - position);
		rows.push_back(parseNumbers(row));
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return rows;
}

std::string jsonMatrix(const std::vector<std::vector<double>>& matrix) {
	std::string result = "[";
	for (std::size_t i = 0; i < matrix.size(); ++i) {
		if (i) result += ',';
		result += jsonNumbers(matrix[i]);
	}
	return result + ']';
}

std::string laSpaceVectors(double ax, double ay, double az, double bx, double by, double bz) {
	try {
		using namespace aksiomat::linear_algebra;
		const Vector3D a{ax, ay, az}, b{bx, by, bz};
		const auto analysisA = SpaceVectorsPlanes::analyze(a);
		const auto crossResult = SpaceVectorsPlanes::cross(a, b);
		return "{\"magnitudeA\":" + formatDouble(analysisA.magnitude) +
			",\"dot\":" + formatDouble(SpaceVectorsPlanes::dot(a, b)) +
			",\"cross\":{\"x\":" + formatDouble(crossResult.crossProduct.x) +
			",\"y\":" + formatDouble(crossResult.crossProduct.y) +
			",\"z\":" + formatDouble(crossResult.crossProduct.z) + "}" +
			",\"crossMagnitude\":" + formatDouble(crossResult.parallelepipedVolumeHint) +
			",\"steps\":" + jsonSteps(crossResult.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laPlaneFromPoints(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto result = SpaceVectorsPlanes::planeFromThreePoints({x1, y1, z1}, {x2, y2, z2}, {x3, y3, z3});
		return "{\"normalX\":" + formatDouble(result.normalX) +
			",\"normalY\":" + formatDouble(result.normalY) +
			",\"normalZ\":" + formatDouble(result.normalZ) +
			",\"constant\":" + formatDouble(result.constant) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laLineVsPlane(double x0, double y0, double z0, double dx, double dy, double dz,
	double normalX, double normalY, double normalZ, double constant) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto result = SpaceVectorsPlanes::lineVsPlane({x0, y0, z0}, {dx, dy, dz}, normalX, normalY, normalZ, constant);
		std::string intersectionJson = "null";
		if (result.intersectionPoint) {
			intersectionJson = "{\"x\":" + formatDouble(result.intersectionPoint->x) +
				",\"y\":" + formatDouble(result.intersectionPoint->y) +
				",\"z\":" + formatDouble(result.intersectionPoint->z) + "}";
		}
		return "{\"lineLiesOnPlane\":" + std::string(result.lineLiesOnPlane ? "true" : "false") +
			",\"lineParallelToPlane\":" + std::string(result.lineParallelToPlane ? "true" : "false") +
			",\"intersects\":" + std::string(result.intersects ? "true" : "false") +
			",\"intersectionPoint\":" + intersectionJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laClassifyQuadric(std::string mode, double a, double b, double c) {
	try {
		using namespace aksiomat::linear_algebra;
		QuadricClassificationResult result = mode == "paraboloid"
			? Quadrics::classifyParaboloid(a, b, c)
			: Quadrics::classifyCentral(a, b, c, 0.0);
		return "{\"typeName\":" + jsonString(result.typeName) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laClassifyQuadricCentral(double a, double b, double c, double d) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto result = Quadrics::classifyCentral(a, b, c, d);
		return "{\"typeName\":" + jsonString(result.typeName) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laMatrixDeterminant(std::string matrixText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto matrix = parseRows(matrixText);
		const auto result = Matrices::determinant(matrix);
		return "{\"determinant\":" + formatDouble(result.determinant) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laMatrixInverse(std::string matrixText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto matrix = parseRows(matrixText);
		const auto result = Matrices::inverse(matrix);
		return "{\"invertible\":" + std::string(result.invertible ? "true" : "false") +
			",\"determinant\":" + formatDouble(result.determinant) +
			",\"inverse\":" + jsonMatrix(result.inverse) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laMatrixMultiply(std::string leftText, std::string rightText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto left = parseRows(leftText);
		const auto right = parseRows(rightText);
		const auto result = Matrices::multiply(left, right);
		return "{\"matrix\":" + jsonMatrix(result.matrix) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laMatrixTransform(std::string matrixText, std::string vectorText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto matrix = parseRows(matrixText);
		const auto vector = parseNumbers(vectorText);
		const auto result = Matrices::applyTransformation(matrix, vector);
		return "{\"transformedVector\":" + jsonNumbers(result.transformedVector) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laEigenSymmetric2x2(double a11, double a12, double a22) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto result = EigenAnalysis::analyzeSymmetric2x2(a11, a12, a22);
		std::string pairsJson = "[";
		for (std::size_t i = 0; i < result.eigenPairs.size(); ++i) {
			if (i) pairsJson += ',';
			pairsJson += "{\"eigenvalue\":" + formatDouble(result.eigenPairs[i].eigenvalue) +
				",\"eigenvector\":" + jsonNumbers(result.eigenPairs[i].eigenvector) + "}";
		}
		pairsJson += ']';
		return "{\"eigenPairs\":" + pairsJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laEigenSymmetric3x3(std::string matrixText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto matrix = parseRows(matrixText);
		const auto result = EigenAnalysis::analyzeSymmetric3x3(matrix);
		std::string pairsJson = "[";
		for (std::size_t i = 0; i < result.eigenPairs.size(); ++i) {
			if (i) pairsJson += ',';
			pairsJson += "{\"eigenvalue\":" + formatDouble(result.eigenPairs[i].eigenvalue) +
				",\"eigenvector\":" + jsonNumbers(result.eigenPairs[i].eigenvector) + "}";
		}
		pairsJson += ']';
		return "{\"eigenPairs\":" + pairsJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laLinearIndependence(std::string vectorsText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto vectors = parseRows(vectorsText);
		const auto result = VectorSpaces::checkLinearIndependence(vectors);
		return "{\"linearlyIndependent\":" + std::string(result.linearlyIndependent ? "true" : "false") +
			",\"rank\":" + std::to_string(result.rank) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string laExtractBasis(std::string vectorsText) {
	try {
		using namespace aksiomat::linear_algebra;
		const auto vectors = parseRows(vectorsText);
		const auto result = VectorSpaces::extractBasis(vectors);
		return "{\"basisVectors\":" + jsonMatrix(result.basisVectors) +
			",\"dimension\":" + std::to_string(result.dimension) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

// Skup zapisan kao "1,2,3" (prazan tekst znaci prazan skup).
std::vector<long long> parseIntSet(const std::string& text) {
	std::vector<long long> values;
	if (text.empty()) return values;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(',', position);
		const std::string token = text.substr(position, end == std::string::npos ? end : end - position);
		if (token.empty()) throw std::invalid_argument("Skup sadrzi praznu vrijednost");
		std::size_t parsed = 0;
		const long long value = std::stoll(token, &parsed);
		if (parsed != token.size()) throw std::invalid_argument("Neispravan element skupa");
		values.push_back(value);
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return values;
}

// Uredeni parovi zapisani kao "1-2;2-3" (svaki par odvojen znakom '-', parovi znakom ';').
std::vector<std::pair<long long, long long>> parseIntPairs(const std::string& text) {
	std::vector<std::pair<long long, long long>> pairs;
	if (text.empty()) return pairs;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(';', position);
		const std::string token = text.substr(position, end == std::string::npos ? end : end - position);
		const std::size_t dash = token.find('-');
		if (dash == std::string::npos) throw std::invalid_argument("Par mora biti u obliku x-y");
		const long long first = std::stoll(token.substr(0, dash));
		const long long second = std::stoll(token.substr(dash + 1));
		pairs.emplace_back(first, second);
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return pairs;
}

std::string jsonIntSet(const std::vector<long long>& values) {
	std::string result = "[";
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (i) result += ',';
		result += std::to_string(values[i]);
	}
	return result + ']';
}

// Popis susjeda po vrhu zapisan kao "1,2;0,2;0,1" (vrhovi indeksirani od 0).
std::vector<std::vector<int>> parseAdjacencyList(const std::string& text) {
	std::vector<std::vector<int>> adjacency;
	std::size_t position = 0;
	while (position <= text.size()) {
		const std::size_t end = text.find(';', position);
		const std::string row = text.substr(position, end == std::string::npos ? end : end - position);
		std::vector<int> neighbors;
		if (!row.empty()) {
			std::size_t rowPos = 0;
			while (rowPos <= row.size()) {
				const std::size_t rowEnd = row.find(',', rowPos);
				const std::string token = row.substr(rowPos, rowEnd == std::string::npos ? rowEnd : rowEnd - rowPos);
				if (token.empty()) throw std::invalid_argument("Popis susjeda sadrzi praznu vrijednost");
				neighbors.push_back(std::stoi(token));
				if (rowEnd == std::string::npos) break;
				rowPos = rowEnd + 1;
			}
		}
		adjacency.push_back(std::move(neighbors));
		if (end == std::string::npos) break;
		position = end + 1;
	}
	return adjacency;
}

std::string discreteMathSetOperation(std::string mode, std::string setAText, std::string setBText) {
	try {
		using namespace aksiomat::discrete_math;
		const auto a = parseIntSet(setAText);
		const auto b = parseIntSet(setBText);
		SetOperationResult result = mode == "union" ? SetsRelations::unionOf(a, b)
			: mode == "intersection" ? SetsRelations::intersectionOf(a, b)
			: mode == "difference" ? SetsRelations::differenceOf(a, b)
			: SetsRelations::symmetricDifferenceOf(a, b);
		return "{\"result\":" + jsonIntSet(result.result) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathRelationProperties(std::string domainText, std::string pairsText) {
	try {
		using namespace aksiomat::discrete_math;
		const auto domain = parseIntSet(domainText);
		const auto pairs = parseIntPairs(pairsText);
		const auto result = SetsRelations::analyzeRelation(domain, pairs);
		auto boolStr = [](bool value) { return value ? "true" : "false"; };
		return std::string("{\"reflexive\":") + boolStr(result.reflexive) +
			",\"symmetric\":" + boolStr(result.symmetric) +
			",\"antisymmetric\":" + boolStr(result.antisymmetric) +
			",\"transitive\":" + boolStr(result.transitive) +
			",\"isEquivalence\":" + boolStr(result.isEquivalence) +
			",\"isPartialOrder\":" + boolStr(result.isPartialOrder) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathGraphAnalyze(std::string adjacencyText, bool directed) {
	try {
		using namespace aksiomat::discrete_math;
		const auto adjacency = parseAdjacencyList(adjacencyText);
		const auto result = GraphTheory::analyze(adjacency, directed);
		std::string degreesJson = "[";
		for (std::size_t i = 0; i < result.degrees.size(); ++i) {
			if (i) degreesJson += ',';
			degreesJson += std::to_string(result.degrees[i]);
		}
		degreesJson += ']';
		auto boolStr = [](bool value) { return value ? "true" : "false"; };
		return std::string("{\"degrees\":") + degreesJson +
			",\"connected\":" + boolStr(result.connected) +
			",\"eulerian\":" + boolStr(result.eulerian) +
			",\"bipartite\":" + boolStr(result.bipartite) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathShortestPath(std::string adjacencyText, int source, int target) {
	try {
		using namespace aksiomat::discrete_math;
		const auto adjacency = parseAdjacencyList(adjacencyText);
		const auto result = GraphTheory::shortestPath(adjacency, source, target);
		std::string pathJson = "[";
		for (std::size_t i = 0; i < result.path.size(); ++i) {
			if (i) pathJson += ',';
			pathJson += std::to_string(result.path[i]);
		}
		pathJson += ']';
		return std::string("{\"reachable\":") + (result.reachable ? "true" : "false") +
			",\"distance\":" + std::to_string(result.distance) +
			",\"path\":" + pathJson +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathSolveRecurrence(double p, double q, double a0, double a1) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = Recurrences::solveLinearSecondOrder(p, q, a0, a1);
		return "{\"root1\":" + formatDouble(result.root1) +
			",\"root2\":" + formatDouble(result.root2) +
			",\"coefficient1\":" + formatDouble(result.coefficient1) +
			",\"coefficient2\":" + formatDouble(result.coefficient2) +
			",\"repeatedRoot\":" + std::string(result.repeatedRoot ? "true" : "false") +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathRecurrenceTerms(double p, double q, double a0, double a1, int count) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = Recurrences::generateTerms(p, q, a0, a1, count);
		return "{\"terms\":" + jsonNumbers(result.terms) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathInclusionExclusionTwo(double sizeA, double sizeB, double sizeIntersection) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = AdvancedCombinatorics::unionSizeTwoSets(
			static_cast<long long>(sizeA), static_cast<long long>(sizeB), static_cast<long long>(sizeIntersection));
		return "{\"unionSize\":" + std::to_string(result.unionSize) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathInclusionExclusionThree(
	double sizeA, double sizeB, double sizeC,
	double sizeAB, double sizeAC, double sizeBC, double sizeABC) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = AdvancedCombinatorics::unionSizeThreeSets(
			static_cast<long long>(sizeA), static_cast<long long>(sizeB), static_cast<long long>(sizeC),
			static_cast<long long>(sizeAB), static_cast<long long>(sizeAC), static_cast<long long>(sizeBC), static_cast<long long>(sizeABC));
		return "{\"unionSize\":" + std::to_string(result.unionSize) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathPigeonhole(double items, double holes) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = AdvancedCombinatorics::pigeonhole(static_cast<long long>(items), static_cast<long long>(holes));
		return "{\"minimumPerHole\":" + std::to_string(result.minimumPerHole) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string discreteMathDerangements(double n) {
	try {
		using namespace aksiomat::discrete_math;
		const auto result = AdvancedCombinatorics::derangements(static_cast<long long>(n));
		return "{\"derangements\":" + std::to_string(result.derangements) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) { return std::string("GRESKA: ") + e.what(); }
}

std::string sequenceAnalyze(std::string terms) {
	try {
		const auto values = parseNumbers(terms);
		const auto result = aksiomat::sequences::SequenceAnalysis::analyze(values);
		const char* monotonicity = result.monotonicity == aksiomat::sequences::Monotonicity::StrictlyIncreasing ? "strictlyIncreasing" :
			result.monotonicity == aksiomat::sequences::Monotonicity::NonDecreasing ? "nonDecreasing" :
			result.monotonicity == aksiomat::sequences::Monotonicity::Constant ? "constant" :
			result.monotonicity == aksiomat::sequences::Monotonicity::NonIncreasing ? "nonIncreasing" :
			result.monotonicity == aksiomat::sequences::Monotonicity::StrictlyDecreasing ? "strictlyDecreasing" : "notMonotonic";
		return std::string("{\"terms\":") + jsonNumbers(values) + ",\"differences\":" + jsonNumbers(result.differences) +
			",\"partialSums\":" + jsonPartialSums(values) + ",\"monotonicity\":\"" + monotonicity +
			"\",\"minimum\":" + formatDouble(result.minimum) + ",\"maximum\":" + formatDouble(result.maximum) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string sequenceArithmetic(double firstTerm, double difference, int termCount, double searchedValue) {
	try {
		if (termCount <= 0) throw std::invalid_argument("Broj clanova mora biti pozitivan");
		const aksiomat::sequences::ArithmeticSequence sequence(firstTerm, difference);
		const auto terms = sequence.generate(static_cast<std::size_t>(termCount));
		const auto index = sequence.indexOf(searchedValue);
		return std::string("{\"terms\":") + jsonNumbers(terms) + ",\"partialSums\":" + jsonPartialSums(terms) +
			",\"lastTerm\":" + formatDouble(terms.back()) + ",\"sum\":" + formatDouble(sequence.partialSum(terms.size())) +
			",\"foundIndex\":" + (index ? std::to_string(*index) : "null") + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string sequenceGeometric(double firstTerm, double ratio, int termCount, double searchedValue) {
	try {
		if (termCount <= 0) throw std::invalid_argument("Broj clanova mora biti pozitivan");
		const aksiomat::sequences::GeometricSequence sequence(firstTerm, ratio);
		const auto terms = sequence.generate(static_cast<std::size_t>(termCount));
		const auto index = sequence.indexOf(searchedValue);
		const auto infinite = sequence.infiniteSum();
		return std::string("{\"terms\":") + jsonNumbers(terms) + ",\"partialSums\":" + jsonPartialSums(terms) +
			",\"lastTerm\":" + formatDouble(terms.back()) + ",\"sum\":" + formatDouble(sequence.partialSum(terms.size())) +
			",\"foundIndex\":" + (index ? std::to_string(*index) : "null") +
			",\"converges\":" + (infinite.converges ? "true" : "false") +
			",\"infiniteSum\":" + (infinite.sum ? formatDouble(*infinite.sum) : "null") + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string sequenceRecurrence(std::string type, double first, double second, int termCount) {
	try {
		if (termCount <= 0) throw std::invalid_argument("Broj clanova mora biti pozitivan");
		const auto count = static_cast<std::size_t>(termCount);
		std::vector<double> terms;
		bool equivalent = true;
		if (type == "arithmetic") {
			const auto result = aksiomat::sequences::Recurrences::compareArithmetic(first, second, count);
			terms = result.recursiveTerms;
			equivalent = result.equivalent;
		} else if (type == "geometric") {
			const auto result = aksiomat::sequences::Recurrences::compareGeometric(first, second, count);
			terms = result.recursiveTerms;
			equivalent = result.equivalent;
		} else if (type == "fibonacci") {
			terms = aksiomat::sequences::Recurrences::fibonacci(first, second, count);
		} else throw std::invalid_argument("Nepoznata rekurzija");
		return std::string("{\"terms\":") + jsonNumbers(terms) + ",\"partialSums\":" + jsonPartialSums(terms) +
			",\"equivalent\":" + (equivalent ? "true" : "false") + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string sequenceApplication(std::string type, double principal, double rate, int periods, int frequency) {
	try {
		if (periods <= 0 || frequency <= 0) throw std::invalid_argument("Razdoblja i ucestalost moraju biti pozitivni");
		if (type == "amortization") {
			const auto result = aksiomat::sequences::Applications::amortization(principal, rate,
				static_cast<std::size_t>(periods), static_cast<std::size_t>(frequency));
			std::vector<double> balances{principal};
			for (const auto& payment : result.schedule) balances.push_back(payment.remainingBalance);
			return std::string("{\"periodicPayment\":") + formatDouble(result.periodicPayment) +
				",\"totalPaid\":" + formatDouble(result.totalPaid) + ",\"totalInterest\":" + formatDouble(result.totalInterest) +
				",\"values\":" + jsonNumbers(balances) + '}';
		}
		aksiomat::sequences::GrowthResult result;
		if (type == "simpleInterest") result = aksiomat::sequences::Applications::simpleInterest(principal, rate, periods);
		else if (type == "compoundInterest") result = aksiomat::sequences::Applications::compoundInterest(principal, rate, periods, frequency);
		else if (type == "population") result = aksiomat::sequences::Applications::populationGrowth(principal, rate, periods);
		else if (type == "percentage") result = aksiomat::sequences::Applications::repeatedPercentage(principal, rate, periods);
		else throw std::invalid_argument("Nepoznata primjena niza");
		return std::string("{\"initialValue\":") + formatDouble(result.initialValue) +
			",\"finalValue\":" + formatDouble(result.finalValue) + ",\"totalChange\":" + formatDouble(result.totalChange) +
			",\"values\":" + jsonNumbers(result.values) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Parsira formulu i vraća njen normalizirani prikaz ili "GRESKA: ...".
std::string logicToString(std::string formula) {
	try {
		return aksiomat::LogicParser::parse(formula)->toString();
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraSimplify(std::string expression) {
	try {
		const auto result = aksiomat::algebra::AlgebraSimplifier::simplify(
			aksiomat::algebra::AlgebraParser::parse(expression));
		return "{\"result\":" + jsonString(aksiomat::algebra::AlgebraFormatter::format(result.expression)) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraSolveEquation(std::string equation) {
	try {
		const auto result = aksiomat::algebra::EquationSolver::solve(equation);
		const char* type = result.type == aksiomat::algebra::EquationSolutionType::Unique ? "unique" :
			result.type == aksiomat::algebra::EquationSolutionType::Infinite ? "infinite" : "none";
		return std::string("{\"type\":\"") + type + "\",\"value\":" + formatDouble(result.value) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraSolveInequality(std::string inequality) {
	try {
		const auto result = aksiomat::algebra::InequalitySolver::solve(inequality);
		const char* type = result.type == aksiomat::algebra::InequalitySolutionType::Interval ? "interval" :
			result.type == aksiomat::algebra::InequalitySolutionType::AllReal ? "all" : "empty";
		return std::string("{\"type\":\"") + type + "\",\"boundary\":" + formatDouble(result.boundary) +
			",\"relation\":" + jsonString(result.relation) + ",\"interval\":" + jsonString(result.interval) +
			",\"steps\":" + jsonSteps(result.steps) + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraSolveSystem(std::string first, std::string second) {
	try {
		const auto result = aksiomat::algebra::LinearSystemSolver::solve(first, second);
		const char* type = result.type == aksiomat::algebra::LinearSystemSolutionType::Unique ? "unique" :
			result.type == aksiomat::algebra::LinearSystemSolutionType::Infinite ? "infinite" : "none";
		return std::string("{\"type\":\"") + type + "\",\"x\":" + formatDouble(result.x) +
			",\"y\":" + formatDouble(result.y) + ",\"steps\":" + jsonSteps(result.steps) +
			",\"methods\":{\"cramer\":" + jsonSteps(result.cramerSteps) +
			",\"substitution\":" + jsonSteps(result.substitutionSteps) +
			",\"elimination\":" + jsonSteps(result.eliminationSteps) + "}}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraAnalyzePolynomial(std::string expression) {
	try {
		const auto polynomial = aksiomat::algebra::Polynomial::parse(expression);
		std::string roots = "[";
		if (polynomial.degree() <= 2) {
			const auto values = polynomial.realRoots();
			for (std::size_t index = 0; index < values.size(); ++index) {
				if (index) roots += ',';
				roots += formatDouble(values[index]);
			}
		}
		std::string discriminant = "null";
		if (const auto value = polynomial.discriminant()) discriminant = formatDouble(*value);
		std::string vertex = "null";
		if (const auto point = polynomial.vertex()) {
			vertex = "{\"x\":" + formatDouble(point->x) + ",\"y\":" + formatDouble(point->y) + '}';
		}
		std::string factorized = "null";
		if (const auto value = polynomial.factorizedForm()) factorized = jsonString(*value);
		return "{\"normalized\":" + jsonString(polynomial.toString()) + ",\"degree\":" +
			std::to_string(polynomial.degree()) + ",\"derivative\":" + jsonString(polynomial.derivative().toString()) +
			",\"roots\":" + roots + "],\"discriminant\":" + discriminant +
			",\"vertex\":" + vertex + ",\"factorized\":" + factorized + '}';
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string algebraAnalyzeFunction(std::string expression, double minX, double maxX, unsigned sampleCount) {
	try {
		const auto analysis = aksiomat::algebra::FunctionAnalyzer::analyze(expression, minX, maxX, sampleCount);
		std::string result = "{\"normalized\":" + jsonString(analysis.normalized) + ",\"domain\":" +
			jsonString(analysis.domain) + ",\"degree\":" + std::to_string(analysis.degree) +
			",\"behavior\":" + jsonString(analysis.behavior) + ",\"yIntercept\":{" +
			"\"x\":" + formatDouble(analysis.yIntercept.x) + ",\"y\":" + formatDouble(analysis.yIntercept.y) +
			"},\"xIntercepts\":[";
		for (std::size_t index = 0; index < analysis.xIntercepts.size(); ++index) {
			if (index) result += ',';
			result += "{\"x\":" + formatDouble(analysis.xIntercepts[index].x) + ",\"y\":0}";
		}
		result += "],\"vertex\":";
		if (analysis.vertex) result += "{\"x\":" + formatDouble(analysis.vertex->x) +
			",\"y\":" + formatDouble(analysis.vertex->y) + '}';
		else result += "null";
		result += ",\"samples\":[";
		for (std::size_t index = 0; index < analysis.samples.size(); ++index) {
			if (index) result += ',';
			result += "[" + formatDouble(analysis.samples[index].x) + ',' + formatDouble(analysis.samples[index].y) + ']';
		}
		return result + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string logicClassification(std::string formula) {
	try {
		const auto result = aksiomat::LogicAnalysis::classify(aksiomat::LogicParser::parse(formula));
		return std::string("{\"tautology\":") + (result.tautology ? "true" : "false") +
			   ",\"contradiction\":" + (result.contradiction ? "true" : "false") +
			   ",\"satisfiable\":" + (result.satisfiable ? "true" : "false") +
			   ",\"contingent\":" + (result.contingent ? "true" : "false") + "}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string logicNormalForms(std::string formula) {
	try {
		auto expression = aksiomat::LogicParser::parse(formula);
		return "{\"nnf\":\"" + aksiomat::NormalForms::toNnf(expression)->toString() +
			   "\",\"transformedCnf\":\"" + aksiomat::NormalForms::toCnf(expression)->toString() +
			   "\",\"transformedDnf\":\"" + aksiomat::NormalForms::toDnf(expression)->toString() +
			   "\",\"canonicalCnf\":\"" + aksiomat::NormalForms::toCanonicalCnf(expression)->toString() +
			   "\",\"canonicalDnf\":\"" + aksiomat::NormalForms::toCanonicalDnf(expression)->toString() + "\"}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Vraća varijable formule odvojene zarezom (npr. "p,q,r") ili "GRESKA: ...".
std::string logicVariables(std::string formula) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		std::string result;
		for (const auto& name : aksiomat::LogicParser::collectVariables(expr)) {
			if (!result.empty()) {
				result += ',';
			}
			result += name;
		}
		return result;
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira formulu uz valuaciju oblika "p=1,q=0"; vraća "T", "F" ili "GRESKA: ...".
std::string logicEvaluate(std::string formula, std::string valuation) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		aksiomat::LogicExpression::Valuation values;
		std::size_t pos = 0;
		while (pos < valuation.size()) {
			std::size_t eq = valuation.find('=', pos);
			if (eq == std::string::npos) {
				break;
			}
			std::size_t end = valuation.find(',', eq);
			if (end == std::string::npos) {
				end = valuation.size();
			}
			values[valuation.substr(pos, eq - pos)] = (valuation.substr(eq + 1, end - eq - 1) == "1");
			pos = end + 1;
		}
		return expr->evaluate(values) ? "T" : "F";
	} catch (const std::out_of_range&) {
		return "GRESKA: Nedostaje vrijednost varijable";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira aritmetički izraz; vraća rezultat kao string ili poruku greške s prefiksom "GRESKA: ".
std::string evaluateArithmetic(std::string expression) {
	try {
		return formatDouble(aksiomat::Arithmetic::evaluate(expression));
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string rationalCalculate(std::string lhsText, std::string operation, std::string rhsText) {
	try {
		const auto lhs = aksiomat::Rational::parse(lhsText);
		const auto rhs = aksiomat::Rational::parse(rhsText);
		aksiomat::Rational result;
		if (operation == "+") result = lhs + rhs;
		else if (operation == "-") result = lhs - rhs;
		else if (operation == "*") result = lhs * rhs;
		else if (operation == "/") result = lhs / rhs;
		else throw std::invalid_argument("Nepoznata operacija razlomaka");
		return "{\"exact\":\"" + result.toString() + "\",\"mixed\":\"" +
			   result.toMixedString() + "\",\"decimal\":\"" + formatDouble(result.toDouble()) + "\"}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string rationalFromDecimal(std::string value) {
	try {
		return aksiomat::Rational::fromDecimal(value).toString();
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string percentageCalculate(std::string operation, double first, double second) {
	try {
		double result = 0.0;
		if (operation == "of") result = aksiomat::Percentages::of(first, second);
		else if (operation == "increase") result = aksiomat::Percentages::increase(first, second);
		else if (operation == "decrease") result = aksiomat::Percentages::decrease(first, second);
		else if (operation == "ratio") result = aksiomat::Percentages::ratio(first, second);
		else if (operation == "originalIncrease") result = aksiomat::Percentages::originalBeforeIncrease(first, second);
		else if (operation == "originalDecrease") result = aksiomat::Percentages::originalBeforeDecrease(first, second);
		else throw std::invalid_argument("Nepoznata postotna operacija");
		return formatDouble(result);
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string numberTheoryAnalyze(std::string valueText) {
	try {
		const auto value = static_cast<std::uint64_t>(std::stoull(valueText));
		std::string result = std::string("{\"prime\":") + (aksiomat::NumberTheory::isPrime(value) ? "true" : "false") +
							 ",\"divisors\":[";
		const auto divisors = aksiomat::NumberTheory::divisors(value);
		for (std::size_t i = 0; i < divisors.size(); ++i) {
			if (i) result += ',';
			result += std::to_string(divisors[i]);
		}
		result += "],\"factors\":[";
		if (value >= 2) {
			const auto factors = aksiomat::NumberTheory::factorize(value);
			for (std::size_t i = 0; i < factors.size(); ++i) {
				if (i) result += ',';
				result += "{\"prime\":" + std::to_string(factors[i].prime) +
						  ",\"exponent\":" + std::to_string(factors[i].exponent) + '}';
			}
		}
		return result + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string numberTheoryGcdLcm(std::string lhsText, std::string rhsText) {
	try {
		const auto lhs = static_cast<std::uint64_t>(std::stoull(lhsText));
		const auto rhs = static_cast<std::uint64_t>(std::stoull(rhsText));
		return "{\"gcd\":\"" + std::to_string(aksiomat::NumberTheory::gcd(lhs, rhs)) +
			   "\",\"lcm\":\"" + std::to_string(aksiomat::NumberTheory::lcm(lhs, rhs)) + "\"}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

std::string convertNumeralSystem(std::string value, unsigned fromBase, unsigned toBase) {
	try {
		return aksiomat::NumeralSystems::convert(value, fromBase, toBase);
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Generira tablicu istinitosti kao JSON: {"variables":[...],"rows":[{"values":[0,1,...],"result":1},...]}
// ili "GRESKA: ...".
std::string truthTable(std::string formula) {
	try {
		auto expr = aksiomat::LogicParser::parse(formula);
		auto table = aksiomat::TruthTable::generate(expr);
		std::string json = "{\"variables\":[";
		for (std::size_t i = 0; i < table.variables.size(); ++i) {
			if (i > 0) json += ',';
			json += '"' + table.variables[i] + '"';
		}
		json += "],\"rows\":[";
		for (std::size_t r = 0; r < table.rows.size(); ++r) {
			if (r > 0) json += ',';
			json += "{\"values\":[";
			for (std::size_t i = 0; i < table.variables.size(); ++i) {
				if (i > 0) json += ',';
				json += table.rows[r].valuation.at(table.variables[i]) ? '1' : '0';
			}
			json += "],\"result\":";
			json += table.rows[r].result ? '1' : '0';
			json += '}';
		}
		return json + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Provjerava logičku ekvivalenciju dviju formula; vraća "T", "F" ili "GRESKA: ...".
std::string logicEquivalent(std::string lhs, std::string rhs) {
	try {
		return aksiomat::TruthTable::areEquivalent(aksiomat::LogicParser::parse(lhs),
												   aksiomat::LogicParser::parse(rhs))
				   ? "T" : "F";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Parsira predikatnu formulu i vraća normalizirani prikaz ili "GRESKA: ...".
std::string predicateToString(std::string formula) {
	try {
		return aksiomat::PredicateParser::parse(formula)->toString();
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Vraća strukturirani opis predikata i slobodnih varijabli kao JSON.
std::string predicatePredicates(std::string formula) {
	try {
		auto expr = aksiomat::PredicateParser::parse(formula);
		std::string result = "{\"predicates\":[";
		bool first = true;
		for (const auto& [name, arity] : aksiomat::PredicateParser::collectPredicates(expr)) {
			if (!first) result += ',';
			first = false;
			result += "{\"name\":\"" + name + "\",\"arity\":" + std::to_string(arity) + '}';
		}
		result += "],\"freeVariables\":[";
		first = true;
		for (const auto& name : aksiomat::PredicateParser::collectFreeVariables(expr)) {
			if (!first) result += ',';
			first = false;
			result += '"' + name + '"';
		}
		result += "],\"boundVariables\":[";
		first = true;
		for (const auto& name : aksiomat::PredicateParser::collectBoundVariables(expr)) {
			if (!first) result += ',';
			first = false;
			result += '"' + name + '"';
		}
		return result + "]}";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

// Evaluira predikatnu formulu nad interpretacijom.
// domain: elementi odvojeni zarezom, npr. "1,2,3"
// facts:  činjenice odvojene točka-zarezom, npr. "P(1);Q(1,2);Q(2,3)"
// Vraća "T", "F" ili "GRESKA: ...".
std::string predicateEvaluate(std::string formula, std::string domain, std::string facts) {
	try {
		auto expr = aksiomat::PredicateParser::parse(formula);

		aksiomat::Interpretation interp;
		std::vector<std::string> elements;
		std::size_t pos = 0;
		while (pos <= domain.size()) {
			std::size_t end = domain.find(',', pos);
			if (end == std::string::npos) end = domain.size();
			if (end == pos) throw std::invalid_argument("Domena sadrzi prazan element");
			elements.push_back(domain.substr(pos, end - pos));
			if (end == domain.size()) break;
			pos = end + 1;
		}
		interp.setDomain(std::move(elements));

		pos = 0;
		while (pos < facts.size()) {
			std::size_t end = facts.find(';', pos);
			if (end == std::string::npos) end = facts.size();
			const std::string fact = facts.substr(pos, end - pos);
			pos = end + 1;
			const std::size_t open = fact.find('(');
			const std::size_t close = fact.rfind(')');
			if (open == std::string::npos || close != fact.size() - 1 || close <= open + 1) {
				throw std::invalid_argument("Neispravna cinjenica '" + fact + "'");
			}
			const std::string name = fact.substr(0, open);
			if (name.empty()) throw std::invalid_argument("Cinjenica nema ime predikata");
			std::vector<std::string> args;
			std::size_t argPos = open + 1;
			while (argPos < close) {
				std::size_t argEnd = fact.find(',', argPos);
				if (argEnd == std::string::npos || argEnd > close) argEnd = close;
				if (argEnd == argPos) throw std::invalid_argument("Cinjenica sadrzi prazan argument");
				args.push_back(fact.substr(argPos, argEnd - argPos));
				argPos = argEnd + 1;
			}
			interp.addFact(name, std::move(args));
		}

		return interp.evaluate(expr) ? "T" : "F";
	} catch (const std::exception& e) {
		return std::string("GRESKA: ") + e.what();
	}
}

} // namespace

EMSCRIPTEN_BINDINGS(aksiomat_module) {
	emscripten::function("logicToString", &logicToString);
	emscripten::function("logicVariables", &logicVariables);
	emscripten::function("logicEvaluate", &logicEvaluate);
	emscripten::function("truthTable", &truthTable);
	emscripten::function("logicEquivalent", &logicEquivalent);
	emscripten::function("logicClassification", &logicClassification);
	emscripten::function("logicNormalForms", &logicNormalForms);
	emscripten::function("predicateToString", &predicateToString);
	emscripten::function("predicatePredicates", &predicatePredicates);
	emscripten::function("predicateEvaluate", &predicateEvaluate);
	emscripten::function("evaluateArithmetic", &evaluateArithmetic);
	emscripten::function("rationalCalculate", &rationalCalculate);
	emscripten::function("rationalFromDecimal", &rationalFromDecimal);
	emscripten::function("percentageCalculate", &percentageCalculate);
	emscripten::function("numberTheoryAnalyze", &numberTheoryAnalyze);
	emscripten::function("numberTheoryGcdLcm", &numberTheoryGcdLcm);
	emscripten::function("convertNumeralSystem", &convertNumeralSystem);
	emscripten::function("geometryConvert", &geometryConvert);
	emscripten::function("geometryPlaneShape", &geometryPlaneShape);
	emscripten::function("geometryTriangle", &geometryTriangle);
	emscripten::function("geometrySolid", &geometrySolid);
	emscripten::function("geometryCoordinates", &geometryCoordinates);
	emscripten::function("trigonometryAngle", &trigonometryAngle);
	emscripten::function("trigonometryFunction", &trigonometryFunction);
	emscripten::function("trigonometryRightTriangle", &trigonometryRightTriangle);
	emscripten::function("trigonometryGeneralTriangle", &trigonometryGeneralTriangle);
	emscripten::function("trigonometryIdentity", &trigonometryIdentity);
	emscripten::function("trigonometryEquation", &trigonometryEquation);
	emscripten::function("sequenceAnalyze", &sequenceAnalyze);
	emscripten::function("sequenceArithmetic", &sequenceArithmetic);
	emscripten::function("sequenceGeometric", &sequenceGeometric);
	emscripten::function("sequenceRecurrence", &sequenceRecurrence);
	emscripten::function("sequenceApplication", &sequenceApplication);
	emscripten::function("analyticGeometryVectors", &analyticGeometryVectors);
	emscripten::function("analyticGeometryLines", &analyticGeometryLines);
	emscripten::function("analyticGeometryCircle", &analyticGeometryCircle);
	emscripten::function("analyticGeometryConic", &analyticGeometryConic);
	emscripten::function("exponentialLogPowersRoots", &exponentialLogPowersRoots);
	emscripten::function("exponentialLogFunction", &exponentialLogFunction);
	emscripten::function("exponentialLogLogarithm", &exponentialLogLogarithm);
	emscripten::function("exponentialLogEquation", &exponentialLogEquation);
	emscripten::function("exponentialLogApplication", &exponentialLogApplication);
	emscripten::function("combinatoricsCounting", &combinatoricsCounting);
	emscripten::function("combinatoricsProbability", &combinatoricsProbability);
	emscripten::function("combinatoricsStatistics", &combinatoricsStatistics);
	emscripten::function("combinatoricsVisualization", &combinatoricsVisualization);

	emscripten::function("calculusLimit", &calculusLimit);
	emscripten::function("calculusDerivative", &calculusDerivative);
	emscripten::function("calculusRateOfChange", &calculusRateOfChange);
	emscripten::function("calculusDerivativeApplications", &calculusDerivativeApplications);
	emscripten::function("calculusDefiniteIntegral", &calculusDefiniteIntegral);

	emscripten::function("mathematicalAnalysisFormalLimit", &maFormalLimit);
	emscripten::function("mathematicalAnalysisContinuity", &maContinuity);
	emscripten::function("mathematicalAnalysisNthDerivative", &maNthDerivative);
	emscripten::function("mathematicalAnalysisChainRule", &maChainRule);
	emscripten::function("mathematicalAnalysisImproperIntegral", &maImproperIntegral);
	emscripten::function("mathematicalAnalysisSubstitutionIntegral", &maSubstitutionIntegral);
	emscripten::function("mathematicalAnalysisTaylorSeries", &maTaylorSeries);
	emscripten::function("mathematicalAnalysisPowerSeries", &maPowerSeries);
	emscripten::function("mathematicalAnalysisPartialDerivatives", &maPartialDerivatives);
	emscripten::function("mathematicalAnalysisDirectionalDerivative", &maDirectionalDerivative);
	emscripten::function("mathematicalAnalysisSolveOde", &maSolveOde);

	emscripten::function("algebraSimplify", &algebraSimplify);
	emscripten::function("algebraSolveEquation", &algebraSolveEquation);
	emscripten::function("algebraSolveInequality", &algebraSolveInequality);
	emscripten::function("algebraSolveSystem", &algebraSolveSystem);
	emscripten::function("algebraAnalyzePolynomial", &algebraAnalyzePolynomial);
	emscripten::function("algebraAnalyzeFunction", &algebraAnalyzeFunction);

	emscripten::function("linearAlgebraSpaceVectors", &laSpaceVectors);
	emscripten::function("linearAlgebraPlaneFromPoints", &laPlaneFromPoints);
	emscripten::function("linearAlgebraLineVsPlane", &laLineVsPlane);
	emscripten::function("linearAlgebraClassifyQuadricParaboloid", &laClassifyQuadric);
	emscripten::function("linearAlgebraClassifyQuadricCentral", &laClassifyQuadricCentral);
	emscripten::function("linearAlgebraMatrixDeterminant", &laMatrixDeterminant);
	emscripten::function("linearAlgebraMatrixInverse", &laMatrixInverse);
	emscripten::function("linearAlgebraMatrixMultiply", &laMatrixMultiply);
	emscripten::function("linearAlgebraMatrixTransform", &laMatrixTransform);
	emscripten::function("linearAlgebraEigenSymmetric2x2", &laEigenSymmetric2x2);
	emscripten::function("linearAlgebraEigenSymmetric3x3", &laEigenSymmetric3x3);
	emscripten::function("linearAlgebraLinearIndependence", &laLinearIndependence);
	emscripten::function("linearAlgebraExtractBasis", &laExtractBasis);

	emscripten::function("discreteMathSetOperation", &discreteMathSetOperation);
	emscripten::function("discreteMathRelationProperties", &discreteMathRelationProperties);
	emscripten::function("discreteMathGraphAnalyze", &discreteMathGraphAnalyze);
	emscripten::function("discreteMathShortestPath", &discreteMathShortestPath);
	emscripten::function("discreteMathSolveRecurrence", &discreteMathSolveRecurrence);
	emscripten::function("discreteMathRecurrenceTerms", &discreteMathRecurrenceTerms);
	emscripten::function("discreteMathInclusionExclusionTwo", &discreteMathInclusionExclusionTwo);
	emscripten::function("discreteMathInclusionExclusionThree", &discreteMathInclusionExclusionThree);
	emscripten::function("discreteMathPigeonhole", &discreteMathPigeonhole);
	emscripten::function("discreteMathDerangements", &discreteMathDerangements);
}

#endif // __EMSCRIPTEN__
