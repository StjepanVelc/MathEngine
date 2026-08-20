#include "aksiomat/algebra/FunctionAnalyzer.hpp"

#include <cmath>
#include <stdexcept>

#include "aksiomat/algebra/Polynomial.hpp"

namespace aksiomat::algebra {

FunctionAnalysis FunctionAnalyzer::analyze(const std::string& expression, double minX, double maxX,
	std::size_t sampleCount) {
	if (!std::isfinite(minX) || !std::isfinite(maxX) || minX >= maxX) {
		throw std::invalid_argument("Neispravan interval uzorkovanja");
	}
	if (sampleCount < 2 || sampleCount > 10000) {
		throw std::invalid_argument("Broj uzoraka mora biti izmedu 2 i 10000");
	}
	const auto polynomial = Polynomial::parse(expression);
	FunctionAnalysis analysis;
	analysis.normalized = polynomial.toString();
	analysis.domain = "R";
	analysis.degree = polynomial.degree();
	analysis.yIntercept = {0.0, polynomial.coefficient(0)};
	if (analysis.degree <= 2) {
		for (const double root : polynomial.realRoots()) analysis.xIntercepts.push_back({root, 0.0});
	}
	if (analysis.degree == 1) {
		analysis.behavior = polynomial.coefficient(1) > 0.0 ? "rastuca" : "padajuca";
	} else if (analysis.degree == 2) {
		const double a = polynomial.coefficient(2);
		const double vertexX = -polynomial.coefficient(1) / (2.0 * a);
		analysis.vertex = FunctionPoint{vertexX, polynomial.evaluate(vertexX)};
		analysis.behavior = a > 0.0 ? "pada do vrha, zatim raste" : "raste do vrha, zatim pada";
	} else if (analysis.degree == 0) {
		analysis.behavior = "konstantna";
	} else {
		analysis.behavior = "polinom stupnja " + std::to_string(analysis.degree);
	}
	analysis.samples.reserve(sampleCount);
	const double step = (maxX - minX) / static_cast<double>(sampleCount - 1);
	for (std::size_t index = 0; index < sampleCount; ++index) {
		const double x = minX + static_cast<double>(index) * step;
		analysis.samples.push_back({x, polynomial.evaluate(x)});
	}
	return analysis;
}

} // namespace aksiomat::algebra
