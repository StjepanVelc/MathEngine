#include "aksiomat/discrete_math/Recurrences.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::discrete_math {

namespace {

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

} // namespace

RecurrenceSolveResult Recurrences::solveLinearSecondOrder(double p, double q, double a0, double a1) {
	const double discriminant = p * p + 4.0 * q;
	if (discriminant < 0.0) {
		throw std::invalid_argument("Karakteristicna jednadzba nema realna rjesenja (kompleksni korijeni nisu podrzani).");
	}

	std::vector<std::string> steps{
		"Karakteristicna jednadzba za a_n = " + formatNumber(p) + "*a_(n-1) + " + formatNumber(q) + "*a_(n-2) glasi x^2 - " + formatNumber(p) + "x - " + formatNumber(q) + " = 0.",
		"Diskriminanta D = p^2 + 4q = " + formatNumber(discriminant) + "."
	};

	if (discriminant == 0.0) {
		const double root = p / 2.0;
		if (root == 0.0) {
			throw std::invalid_argument("Rekurzija je degenerirana (korijen jednak nuli).");
		}
		// a_n = (c1 + c2*n) * root^n; a0 = c1; a1 = (c1 + c2) * root.
		const double c1 = a0;
		const double c2 = (a1 / root) - c1;
		steps.push_back("Dvostruki korijen x = " + formatNumber(root) + ", pa je opce rjesenje a_n = (c1 + c2*n) * " + formatNumber(root) + "^n.");
		steps.push_back("Iz pocetnih uvjeta: c1 = " + formatNumber(c1) + ", c2 = " + formatNumber(c2) + ".");
		return RecurrenceSolveResult{root, root, c1, c2, true, std::move(steps)};
	}

	const double sqrtD = std::sqrt(discriminant);
	const double root1 = (p + sqrtD) / 2.0;
	const double root2 = (p - sqrtD) / 2.0;
	if (root1 == root2) {
		throw std::invalid_argument("Rekurzija je degenerirana (podudarni korijeni bez numericke razlike).");
	}
	// a0 = c1 + c2; a1 = c1*root1 + c2*root2.
	const double c1 = (a1 - a0 * root2) / (root1 - root2);
	const double c2 = a0 - c1;

	steps.push_back("Korijeni karakteristicne jednadzbe: x1 = " + formatNumber(root1) + ", x2 = " + formatNumber(root2) + ".");
	steps.push_back("Opce rjesenje: a_n = c1*x1^n + c2*x2^n.");
	steps.push_back("Iz pocetnih uvjeta: c1 = " + formatNumber(c1) + ", c2 = " + formatNumber(c2) + ".");

	return RecurrenceSolveResult{root1, root2, c1, c2, false, std::move(steps)};
}

RecurrenceTermsResult Recurrences::generateTerms(double p, double q, double a0, double a1, int count) {
	if (count < 0) {
		throw std::invalid_argument("Broj clanova ne smije biti negativan.");
	}
	std::vector<double> terms;
	terms.reserve(static_cast<std::size_t>(count));
	if (count > 0) terms.push_back(a0);
	if (count > 1) terms.push_back(a1);
	for (int i = 2; i < count; ++i) {
		terms.push_back(p * terms[i - 1] + q * terms[i - 2]);
	}
	std::vector<std::string> steps{
		"Clanovi rekurzije generirani su izravnom primjenom formule a_n = " + formatNumber(p) + "*a_(n-1) + " + formatNumber(q) + "*a_(n-2)."
	};
	return RecurrenceTermsResult{terms, std::move(steps)};
}

} // namespace aksiomat::discrete_math
