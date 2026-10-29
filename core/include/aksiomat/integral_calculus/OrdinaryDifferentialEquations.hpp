#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

struct OdeSolution {
	std::string type;        // "separable" ili "linear"
	std::string equation;
	std::string general;     // opce rjesenje (eksplicitno ili implicitno) s konstantom C
	std::string particular;  // partikularno rjesenje kad je zadan pocetni uvjet
	bool explicitForm = false;
	bool hasInitial = false;
	double x0 = 0.0;
	double y0 = 0.0;
	double constant = 0.0;   // vrijednost C iz pocetnog uvjeta
	std::vector<double> xs;  // uzorci za crtanje rjesenja
	std::vector<double> ys;
	bool verified = false;
	double residual = 0.0;   // najvece odstupanje pri provjeri
	std::string verification;
	std::vector<std::string> steps;
};

class OrdinaryDifferentialEquations {
public:
	// y' = g(x)·h(y). g je funkcija od x, h funkcija od y. Ako je hasInitial, rjesava Cauchyjev
	// problem y(x0) = y0 i uzorkuje rjesenje do xEnd u `count` tocaka.
	// Baca std::invalid_argument za neispravan unos ili ako integral nije pronaden.
	static OdeSolution separable(const std::string& g, const std::string& h, bool hasInitial, double x0, double y0,
		double xEnd, int count = 80);

	// y' + p(x)·y = q(x) metodom integrirajuceg faktora mu = e^(∫p dx).
	static OdeSolution linearFirstOrder(const std::string& p, const std::string& q, bool hasInitial, double x0,
		double y0, double xEnd, int count = 80);

	// Model rasta/raspada y' = k·y, y(0) = y0 (npr. kamate, stanovnistvo): vraca rjesenje y0·e^(k t).
	static OdeSolution exponentialGrowth(double k, double y0, double tEnd, int count = 80);

	// Logisticki model y' = r·y·(1 - y/K), y(0) = y0 (ogranicen rast; ekonomija, populacije).
	static OdeSolution logistic(double r, double capacity, double y0, double tEnd, int count = 80);
};

} // namespace aksiomat::integral_calculus
