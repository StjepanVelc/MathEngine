#include "aksiomat/integral_calculus/SequenceLimits.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"

namespace aksiomat::integral_calculus {
namespace {

using differential_calculus::evaluate;
using differential_calculus::formatValue;
using differential_calculus::parseFunction;

bool close(double a, double b) { return std::fabs(a - b) <= 1e-9 * (1.0 + std::fabs(a) + std::fabs(b)); }

void requireN(int maxN) {
	if (maxN < 1 || maxN > 1000) throw std::invalid_argument("maxN mora biti izmedu 1 i 1000.");
}

} // namespace

InductionResult SequenceLimits::verifySumIdentity(const std::string& summand, const std::string& closedForm, int maxN) {
	requireN(maxN);
	const auto f = parseFunction(summand, "n");
	const auto g = parseFunction(closedForm, "n");
	InductionResult r;
	r.statement = "sum_{k=1}^{n} (" + summand + " uz n -> k) = " + closedForm;

	double sum = 0.0;
	r.baseCaseHolds = false;
	bool allHold = true;
	for (int n = 1; n <= maxN; ++n) {
		sum += evaluate(f, n);
		const double rhs = evaluate(g, n);
		if (!std::isfinite(sum) || !std::isfinite(rhs)) throw std::invalid_argument("Izraz nije definiran za prirodne brojeve.");
		const bool ok = close(sum, rhs);
		if (n == 1) r.baseCaseHolds = ok;
		if (!ok && allHold) {
			allHold = false;
			r.firstCounterexample = n;
		}
		if (allHold) r.checked = n;
	}
	r.inductionStepVerified = allHold;

	r.steps.push_back("Dokazujemo tvrdnju T(n): " + r.statement + " za sve prirodne brojeve n >= 1 matematickom indukcijom.");
	r.steps.push_back(std::string("1. Baza (n = 1): lijeva strana = ") + formatValue(evaluate(f, 1)) + ", desna strana = " +
		formatValue(evaluate(g, 1)) + (r.baseCaseHolds ? " -> baza vrijedi." : " -> baza NE vrijedi, tvrdnja je kriva."));
	r.steps.push_back("2. Pretpostavka: T(n) vrijedi za neki n, tj. suma do n jednaka je g(n).");
	r.steps.push_back("3. Korak: treba pokazati T(n+1), tj. g(n) + f(n+1) = g(n+1). Uz pretpostavku je suma do n+1 jednaka "
		"g(n) + f(n+1), pa algebarski treba provjeriti da je to g(n+1).");
	if (allHold) {
		r.steps.push_back("Numericka provjera potvrdila je g(n) + f(n+1) = g(n+1) za sve n do " + std::to_string(maxN) +
			". To nije dokaz za sve n, ali ako se razlika g(n+1) - g(n) - f(n+1) algebarski svede na 0, korak je dokazan.");
		r.steps.push_back("Zakljucak: po principu matematicke indukcije T(n) vrijedi za sve n >= 1 (uz algebarsku provjeru koraka).");
	} else {
		r.steps.push_back("Prvi n za koji tvrdnja ne vrijedi je n = " + std::to_string(r.firstCounterexample) +
			", pa je to protuprimjer i indukcijom se tvrdnja ne moze dokazati.");
	}
	return r;
}

InductionResult SequenceLimits::verifyInequality(const std::string& left, const std::string& right, int n0, int maxN) {
	requireN(maxN);
	if (n0 < 0 || n0 > maxN) throw std::invalid_argument("n0 mora biti izmedu 0 i maxN.");
	const auto f = parseFunction(left, "n");
	const auto g = parseFunction(right, "n");
	InductionResult r;
	r.statement = "(" + left + ") <= (" + right + ") za n >= " + std::to_string(n0);
	bool allHold = true;
	for (int n = n0; n <= maxN; ++n) {
		const double a = evaluate(f, n), b = evaluate(g, n);
		if (!std::isfinite(a) || !std::isfinite(b)) throw std::invalid_argument("Izraz nije definiran za prirodne brojeve.");
		const bool ok = a <= b + 1e-9 * (1.0 + std::fabs(b));
		if (n == n0) r.baseCaseHolds = ok;
		if (!ok && allHold) {
			allHold = false;
			r.firstCounterexample = n;
		}
		if (allHold) r.checked = n;
	}
	r.inductionStepVerified = allHold;
	r.steps.push_back("Dokazujemo nejednakost " + r.statement + " indukcijom.");
	r.steps.push_back("1. Baza (n = " + std::to_string(n0) + "): " + formatValue(evaluate(f, n0)) + " <= " + formatValue(evaluate(g, n0)) +
		(r.baseCaseHolds ? " -> vrijedi." : " -> NE vrijedi."));
	r.steps.push_back("2. Pretpostavka: nejednakost vrijedi za neki n >= n0.");
	r.steps.push_back("3. Korak: iz pretpostavke treba izvesti istu nejednakost za n+1 (obicno pomnozimo/zbrojimo pretpostavku "
		"i ocijenimo preostali dio).");
	r.steps.push_back(allHold ? "Numericki je nejednakost potvrdena do n = " + std::to_string(maxN) + "; algebarski korak je ono sto cini dokaz."
							  : "Prvi protuprimjer je n = " + std::to_string(r.firstCounterexample) + ", pa nejednakost ne vrijedi za sve n >= n0.");
	return r;
}

SequenceLimitResult SequenceLimits::analyze(const std::string& expression, double epsilon, int termCount) {
	if (!(epsilon > 0.0) || !std::isfinite(epsilon)) throw std::invalid_argument("Epsilon mora biti pozitivan.");
	if (termCount < 5 || termCount > 500) throw std::invalid_argument("Broj clanova mora biti izmedu 5 i 500.");
	const auto f = parseFunction(expression, "n");
	SequenceLimitResult r;
	r.expression = expression;
	r.epsilon = epsilon;

	for (int n = 1; n <= termCount; ++n) {
		const double v = evaluate(f, n);
		if (!std::isfinite(v)) throw std::invalid_argument("Niz nije definiran za n = " + std::to_string(n) + ".");
		r.terms.push_back(v);
	}
	r.lowerBound = *std::min_element(r.terms.begin(), r.terms.end());
	r.upperBound = *std::max_element(r.terms.begin(), r.terms.end());
	r.bounded = std::fabs(r.lowerBound) < 1e12 && std::fabs(r.upperBound) < 1e12;

	bool inc = true, dec = true;
	for (std::size_t i = 1; i < r.terms.size(); ++i) {
		if (r.terms[i] < r.terms[i - 1] - 1e-12) inc = false;
		if (r.terms[i] > r.terms[i - 1] + 1e-12) dec = false;
	}
	r.monotone = inc || dec;
	r.monotonicity = inc && dec ? "konstantan" : inc ? "rastuci" : dec ? "padajuci" : "nije monoton";

	const double far = std::max(1e6, 1e4 * termCount);
	const double a1 = evaluate(f, far), a2 = evaluate(f, far * 10.0), a3 = evaluate(f, far * 100.0);
	const double aNext = evaluate(f, far + 1.0), aNext2 = evaluate(f, far * 10.0 + 1.0);
	const double tol = 1e-4 * (1.0 + std::fabs(a3));
	const bool neighboursAgree = std::isfinite(aNext) && std::isfinite(aNext2) && std::fabs(a1 - aNext) <= tol * 10.0 &&
		std::fabs(a2 - aNext2) <= tol * 10.0;
	if (std::isfinite(a1) && std::isfinite(a2) && std::isfinite(a3) && neighboursAgree && std::fabs(a2 - a3) <= tol &&
		std::fabs(a1 - a2) <= tol * 10.0) {
		r.converges = true;
		r.limit = differential_calculus::snapToNice(a3);
	} else if (std::fabs(a3) > 1e5 && std::fabs(a3) > std::fabs(a2)) {
		r.divergesToInfinity = true;
	}

	r.steps.push_back("Niz a_n = " + expression + ". Prvih nekoliko clanova: " + formatValue(r.terms[0]) + ", " + formatValue(r.terms[1]) +
		", " + formatValue(r.terms[2]) + ", " + formatValue(r.terms[3]) + ", ...");
	r.steps.push_back("Monotonost: niz je " + r.monotonicity + " (po prvih " + std::to_string(termCount) + " clanova). "
		"Teorem: monoton i ogranicen niz konvergira.");
	r.steps.push_back(std::string("Ogranicenost: ") + (r.bounded ? "svi promatrani clanovi su izmedu " + formatValue(r.lowerBound) + " i " +
		formatValue(r.upperBound) + "." : "clanovi nisu ograniceni."));
	if (r.converges) {
		r.steps.push_back("Za velike n vrijednosti se stabiliziraju: lim a_n = " + formatValue(r.limit) + ".");
		r.steps.push_back("Definicija: lim a_n = L ako za svaki eps > 0 postoji N takav da je |a_n - L| < eps za sve n > N.");
		int N = 0;
		for (int n = termCount; n >= 1; --n) {
			if (std::fabs(evaluate(f, n) - r.limit) >= epsilon) {
				N = n;
				break;
			}
		}
		if (N < termCount) {
			r.epsilonN = N;
			r.steps.push_back("Za eps = " + formatValue(epsilon) + " vrijedi |a_n - " + formatValue(r.limit) + "| < eps za sve n > " + std::to_string(N) + ", pa je N = " +
				std::to_string(N) + ".");
		} else {
			r.steps.push_back("Za eps = " + formatValue(epsilon) + " treba N vece od " + std::to_string(termCount) + ".");
		}
	} else if (r.divergesToInfinity) {
		r.steps.push_back("Clanovi neograniceno rastu po apsolutnoj vrijednosti, pa niz divergira.");
	} else {
		r.steps.push_back("Vrijednosti se ne stabiliziraju, pa niz nema konacan limes (oscilira ili divergira).");
	}
	return r;
}

} // namespace aksiomat::integral_calculus
