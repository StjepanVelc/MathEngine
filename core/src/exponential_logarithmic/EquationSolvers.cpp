#include "aksiomat/exponential_logarithmic/EquationSolvers.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::exponential_logarithmic {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Value must be finite");
}

} // namespace

// base^x * coefficient = target  =>  x = log(target / coefficient) / log(base)
EquationSolution EquationSolvers::solveExponential(double base, double coefficient, double target) {
	requireFinite(base); requireFinite(coefficient); requireFinite(target);
	if (base <= 0.0 || base == 1.0) throw std::invalid_argument("Exponential base must be positive and not equal to one");
	if (coefficient == 0.0) throw std::invalid_argument("Coefficient cannot be zero");
	const double ratio = target / coefficient;
	if (ratio <= 0.0) throw std::invalid_argument("Equation has no real solution for the given target");
	const double solution = std::log(ratio) / std::log(base);
	if (!std::isfinite(solution)) throw std::invalid_argument("Equation solution is not finite");
	return {EquationKind::Exponential, solution};
}

// coefficient * log_base(x) = target  =>  x = base^(target / coefficient)
EquationSolution EquationSolvers::solveLogarithmic(double base, double coefficient, double target) {
	requireFinite(base); requireFinite(coefficient); requireFinite(target);
	if (base <= 0.0 || base == 1.0) throw std::invalid_argument("Logarithm base must be positive and not equal to one");
	if (coefficient == 0.0) throw std::invalid_argument("Coefficient cannot be zero");
	const double solution = std::pow(base, target / coefficient);
	if (!std::isfinite(solution) || solution <= 0.0) throw std::invalid_argument("Equation solution is not finite or positive");
	return {EquationKind::Logarithmic, solution};
}

} // namespace aksiomat::exponential_logarithmic
