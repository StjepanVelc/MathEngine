#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct InductionResult {
	std::string statement;        // npr. "1 + 2 + ... + n = n(n+1)/2"
	bool baseCaseHolds = false;
	bool inductionStepVerified = false;  // provjereno numericki za n = 1..checked (nije dokaz!)
	int checked = 0;
	int firstCounterexample = 0;  // 0 = nema
	std::vector<std::string> steps;
};

struct SequenceLimitResult {
	std::string expression;
	bool converges = false;
	bool divergesToInfinity = false;
	double limit = 0.0;
	bool monotone = false;
	std::string monotonicity;     // "rastuci", "padajuci", "nije monoton"
	bool bounded = false;
	double lowerBound = 0.0;
	double upperBound = 0.0;
	int epsilonN = 0;             // N za zadani epsilon (0 ako nije racunano)
	double epsilon = 0.0;
	std::vector<double> terms;
	std::vector<std::string> steps;
};

class SequenceLimits {
public:
	// Provjera tvrdnje sum_{k=1}^n f(k) = g(n) kao primjer matematicke indukcije: baza, korak i
	// numericka provjera za n = 1..maxN. Napomena: numericka provjera nije dokaz, ali pokazuje strukturu.
	static InductionResult verifySumIdentity(const std::string& summand, const std::string& closedForm, int maxN = 60);

	// Provjera nejednakosti f(n) <= g(n) za n >= n0 uz trazenje prvog protuprimjera.
	static InductionResult verifyInequality(const std::string& left, const std::string& right, int n0, int maxN = 60);

	// Limes niza a_n zadanog formulom u varijabli n: konvergencija, monotonost, ogranicenost i epsilon-N.
	static SequenceLimitResult analyze(const std::string& expression, double epsilon = 0.01, int termCount = 30);
};

} // namespace aksiomat::integral_calculus
