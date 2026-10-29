#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct DefiniteIntegralResult {
	std::string expression;
	double lower = 0.0;
	double upper = 0.0;
	bool symbolic = false;       // true ako je koristen Newton-Leibniz s pronadenom antiderivacijom
	std::string antiderivative;  // F(x) kad je symbolic
	double value = 0.0;          // vrijednost integrala (signed)
	double numericCheck = 0.0;   // neovisna numericka procjena (Simpson)
	bool agrees = false;         // slazu li se symbolic i numeric vrijednost
	std::string method;
	std::vector<std::string> steps;
};

struct AreaBetweenCurvesResult {
	std::string upperCurve;
	std::string lowerCurve;
	double lower = 0.0;
	double upper = 0.0;
	double area = 0.0;
	std::vector<double> crossings;  // x-ovi sjecista unutar [lower, upper]
	std::vector<std::string> steps;
};

enum class ImproperKind { Convergent, Divergent, Unknown };

struct ImproperIntegralInfo {
	std::string expression;
	double lower = 0.0;
	double upper = 0.0;
	bool lowerInfinite = false;
	bool upperInfinite = false;
	ImproperKind kind = ImproperKind::Unknown;
	double value = 0.0;
	std::vector<double> bounds;   // granice b_k za vizualizaciju konvergencije
	std::vector<double> partials; // integral do b_k
	std::vector<std::string> steps;
};

class DefiniteIntegration {
public:
	// Newton-Leibniz: ∫_a^b f dx = F(b) - F(a). Ako antiderivacija nije pronadena, koristi se
	// numericka integracija. Baca std::invalid_argument za neispravan unos ili a == b nedefinirano.
	static DefiniteIntegralResult integrate(const std::string& expression, double a, double b,
		const std::string& variable = "x");

	// Povrsina izmedu krivulja f i g na [a, b] (integral |f - g|), uz pronalazenje sjecista.
	static AreaBetweenCurvesResult areaBetween(const std::string& f, const std::string& g, double a, double b);

	// Nepravi integral na beskonacnom intervalu: oba kraja mogu biti beskonacna (koristi se flag).
	static ImproperIntegralInfo improper(const std::string& expression, double a, bool lowerInfinite,
		double b, bool upperInfinite);

	// Srednja vrijednost funkcije na [a, b] i tocka c iz teorema srednje vrijednosti za integrale.
	static DefiniteIntegralResult meanValue(const std::string& expression, double a, double b);
};

} // namespace aksiomat::integral_calculus
