#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct IndefiniteIntegralResult {
	std::string expression;
	bool found = false;
	std::string antiderivative;      // F(x) bez konstante C
	std::string antiderivativeWithC; // F(x) + C
	std::string method;              // glavna metoda: tablicna, supstitucija, parcijalna, racionalna...
	bool verified = false;           // je li F'(x) = f(x) potvrdeno numericki
	std::string verification;        // izvedeni F'(x) i ishod provjere
	std::string message;             // poruka kad antiderivacija nije pronadena
	std::vector<std::string> steps;
};

class IndefiniteIntegration {
public:
	// Neodredeni integral uz objasnjenje po koracima i provjeru deriviranjem.
	// Ako antiderivaciju nije moguce pronaci, found = false i message objasnjava zasto.
	// Baca std::invalid_argument za neispravan izraz.
	static IndefiniteIntegralResult integrate(const std::string& expression, const std::string& variable = "x");

	// Je li F antiderivacija od f (provjera F' = f na zajednickoj domeni).
	static bool isAntiderivative(const std::string& antiderivative, const std::string& integrand,
		const std::string& variable = "x");
};

} // namespace aksiomat::integral_calculus
