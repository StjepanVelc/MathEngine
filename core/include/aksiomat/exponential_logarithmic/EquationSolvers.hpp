#pragma once

namespace aksiomat::exponential_logarithmic {

enum class EquationKind {
	Exponential,
	Logarithmic
};

struct EquationSolution {
	EquationKind kind;
	double solution;
};

// Rješava jednostavne jednadžbe oblika:
//  - eksponencijalne:  base^(x) * coefficient = target        =>  x
//  - logaritamske:     coefficient * log_base(x) = target     =>  x
class EquationSolvers {
public:
	static EquationSolution solveExponential(double base, double coefficient, double target);
	static EquationSolution solveLogarithmic(double base, double coefficient, double target);
};

} // namespace aksiomat::exponential_logarithmic
