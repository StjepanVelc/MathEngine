// WASM bindings za aksiomat_core — kompajlira se SAMO pod Emscripten toolchainom.
// Native (MSVC) build ovu datoteku ne uključuje (vidi core/CMakeLists.txt).
#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

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

namespace {

std::string formatDouble(double value) {
	std::string text = std::to_string(value);
	text.erase(text.find_last_not_of('0') + 1);
	if (!text.empty() && text.back() == '.') text.pop_back();
	return text;
}

std::string jsonString(const std::string& value);

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
	emscripten::function("algebraSimplify", &algebraSimplify);
	emscripten::function("algebraSolveEquation", &algebraSolveEquation);
	emscripten::function("algebraSolveInequality", &algebraSolveInequality);
	emscripten::function("algebraSolveSystem", &algebraSolveSystem);
	emscripten::function("algebraAnalyzePolynomial", &algebraAnalyzePolynomial);
	emscripten::function("algebraAnalyzeFunction", &algebraAnalyzeFunction);
}

#endif // __EMSCRIPTEN__
