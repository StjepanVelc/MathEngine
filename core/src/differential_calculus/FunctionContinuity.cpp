#include "aksiomat/differential_calculus/FunctionContinuity.hpp"

#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/NumericText.hpp"

namespace aksiomat::differential_calculus {

ContinuityAnalysis FunctionContinuity::analyze(const std::string& expression, double point,
	const std::string& variable) {
	if (!std::isfinite(point)) throw std::invalid_argument("Tocka mora biti konacan broj");
	const auto f = simplify(parseFunction(expression, variable));

	ContinuityAnalysis result;
	result.expression = toString(f);
	result.point = point;
	result.value = ::aksiomat::differential_calculus::evaluate(f, point);
	result.valueDefined = std::isfinite(result.value);
	result.left = RealFunctionLimits::oneSided(f, point, -1);
	result.right = RealFunctionLimits::oneSided(f, point, 1);

	const std::string a = formatValue(point);
	result.steps.push_back("Funkcija f je neprekidna u tocki a ako vrijede tri uvjeta: (1) f(a) je definiran, (2) lijevi i desni limes u a postoje, "
		"(3) oba su jednaka f(a).");
	result.steps.push_back("Uvjet 1: f(" + a + ") " + (result.valueDefined ? "= " + formatValue(result.value) + " je definiran." : "nije definiran."));
	result.steps.push_back("Uvjet 2: lijevi limes = " + RealFunctionLimits::describe(result.left) +
		", desni limes = " + RealFunctionLimits::describe(result.right) + ".");

	const bool leftUndefined = result.left.kind == LimitKind::Undefined;
	const bool rightUndefined = result.right.kind == LimitKind::Undefined;
	const bool leftFinite = result.left.kind == LimitKind::Finite;
	const bool rightFinite = result.right.kind == LimitKind::Finite;

	if (leftUndefined && rightUndefined) {
		result.kind = ContinuityKind::OutsideDomain;
		result.kindId = "outside_domain";
		result.steps.push_back("Funkcija nije definirana ni lijevo ni desno od a, pa neprekidnost u a nema smisla (a nije u domeni).");
	} else if (leftUndefined || rightUndefined) {
		result.kind = ContinuityKind::OutsideDomain;
		result.kindId = "outside_domain";
		result.steps.push_back("Funkcija je definirana samo s jedne strane tocke a (rub domene). Moze biti samo jednostrano neprekidna.");
	} else if (leftFinite && rightFinite) {
		const bool equalSides = std::fabs(result.left.value - result.right.value) <=
			1e-4 * (1.0 + std::fabs(result.left.value));
		if (!equalSides) {
			result.kind = ContinuityKind::Jump;
			result.kindId = "jump";
			result.steps.push_back("Limesi se razlikuju: skok iznosi " + formatValue(result.right.value - result.left.value) +
				". To je prekid prve vrste - skok.");
		} else if (result.valueDefined &&
			std::fabs(result.value - result.left.value) <= 1e-4 * (1.0 + std::fabs(result.value))) {
			result.kind = ContinuityKind::Continuous;
			result.kindId = "continuous";
			result.steps.push_back("Uvjet 3: limes = " + formatValue(result.left.value) + " = f(" + a + "). Sva tri uvjeta vrijede - f je neprekidna u a.");
		} else {
			result.kind = ContinuityKind::Removable;
			result.kindId = "removable";
			result.steps.push_back(std::string("Limes postoji (") + formatValue(result.left.value) + ") ali " +
				(result.valueDefined ? "razlikuje se od f(a) = " + formatValue(result.value) + "." : "f(a) nije definiran.") +
				" To je uklonjiv prekid: dovoljno je (re)definirati f(" + a + ") = " + formatValue(result.left.value) + ".");
		}
	} else if (result.left.kind == LimitKind::DoesNotExist || result.right.kind == LimitKind::DoesNotExist) {
		result.kind = ContinuityKind::Oscillating;
		result.kindId = "oscillating";
		result.steps.push_back("Barem jedan jednostrani limes ne postoji jer vrijednosti osciliraju (npr. sin(1/x) kad x -> 0). To je bitni prekid.");
	} else {
		result.kind = ContinuityKind::Infinite;
		result.kindId = "infinite";
		result.steps.push_back("Barem jedan jednostrani limes je beskonacan: graf ima vertikalnu asimptotu x = " + a + ". To je prekid druge vrste.");
	}
	return result;
}

} // namespace aksiomat::differential_calculus
