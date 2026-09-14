#pragma once

#include <string>
#include <vector>

namespace aksiomat::discrete_math {

struct RecurrenceSolveResult {
	// Zatvoreni oblik a_n = c1*r1^n + c2*r2^n (ili s dodatnim linearnim clanom kad je r1 == r2).
	double root1;
	double root2;
	double coefficient1;
	double coefficient2;
	bool repeatedRoot;
	std::vector<std::string> steps;
};

struct RecurrenceTermsResult {
	std::vector<double> terms;
	std::vector<std::string> steps;
};

class Recurrences {
public:
	// Rjesava homogenu linearnu rekurziju drugog reda a_n = p*a_(n-1) + q*a_(n-2)
	// zadanu pocetnim uvjetima a0, a1, koristeci karakteristicnu jednadzbu x^2 - p*x - q = 0.
	static RecurrenceSolveResult solveLinearSecondOrder(double p, double q, double a0, double a1);
	static RecurrenceTermsResult generateTerms(double p, double q, double a0, double a1, int count);
};

} // namespace aksiomat::discrete_math
