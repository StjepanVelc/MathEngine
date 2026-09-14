#include "aksiomat/linear_algebra/Quadrics.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat::linear_algebra {

namespace {

void requireFinite(double value, const char* name) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(std::string(name) + " mora biti konacan broj.");
	}
}

std::string nameFor(QuadricType type) {
	switch (type) {
		case QuadricType::Ellipsoid: return "elipsoid";
		case QuadricType::HyperboloidOneSheet: return "hiperboloid jedne ljuske";
		case QuadricType::HyperboloidTwoSheets: return "hiperboloid dvije ljuske";
		case QuadricType::EllipticParaboloid: return "elipticki paraboloid";
		case QuadricType::HyperbolicParaboloid: return "hiperbolicki paraboloid (sedlo)";
		case QuadricType::EllipticCone: return "elipticki stozac";
		default: return "nepoznata kvadrika";
	}
}

} // namespace

QuadricClassificationResult Quadrics::classifyCentral(double a, double b, double c, double d) {
	requireFinite(a, "A"); requireFinite(b, "B"); requireFinite(c, "C"); requireFinite(d, "D");
	if (a == 0.0 && b == 0.0 && c == 0.0) {
		throw std::invalid_argument("Barem jedan od koeficijenata A, B, C mora biti razlicit od nule.");
	}

	std::vector<std::string> steps;
	steps.push_back("Kanonski oblik: A*x^2 + B*y^2 + C*z^2 + D = 0 s A=" + std::to_string(a) + ", B=" + std::to_string(b) + ", C=" + std::to_string(c) + ", D=" + std::to_string(d) + ".");

	QuadricType type;
	if (d == 0.0) {
		// Stozac: A*x^2 + B*y^2 + C*z^2 = 0 s mijesanim predznacima.
		const bool allSameSign = (a > 0 && b > 0 && c > 0) || (a < 0 && b < 0 && c < 0);
		if (!allSameSign) {
			type = QuadricType::EllipticCone;
			steps.push_back("D = 0 uz mijesane predznake koeficijenata daje elipticki stozac s vrhom u ishodistu.");
		} else {
			throw std::invalid_argument("Uz D=0 i sve koeficijente istog predznaka jednadzba opisuje samo ishodiste, nije prava kvadrika.");
		}
	} else {
		const int positiveCount = (a > 0) + (b > 0) + (c > 0);
		const int negativeCount = (a < 0) + (b < 0) + (c < 0);
		if (d < 0) {
			if (positiveCount == 3) {
				type = QuadricType::Ellipsoid;
				steps.push_back("Svi koeficijenti pozitivni i D negativan: elipsoid.");
			} else if (positiveCount == 2 && negativeCount == 1) {
				type = QuadricType::HyperboloidOneSheet;
				steps.push_back("Dva pozitivna i jedan negativan koeficijent uz D<0: hiperboloid jedne ljuske.");
			} else if (positiveCount == 1 && negativeCount == 2) {
				type = QuadricType::HyperboloidTwoSheets;
				steps.push_back("Jedan pozitivan i dva negativna koeficijenta uz D<0: hiperboloid dvije ljuske.");
			} else {
				type = QuadricType::Unknown;
				steps.push_back("Kombinacija predznaka ne odgovara standardnom obliku sredisnje kvadrike.");
			}
		} else {
			// D > 0 -> zamijeni ulogu predznaka jer mnozimo jednadzbu s -1 konceptualno.
			if (negativeCount == 3) {
				type = QuadricType::Ellipsoid;
				steps.push_back("Svi koeficijenti negativni i D pozitivan (ekvivalentno mnozenju s -1): elipsoid.");
			} else if (negativeCount == 2 && positiveCount == 1) {
				type = QuadricType::HyperboloidOneSheet;
				steps.push_back("Nakon mnozenja s -1 dobivamo dva pozitivna i jedan negativan koeficijent: hiperboloid jedne ljuske.");
			} else if (negativeCount == 1 && positiveCount == 2) {
				type = QuadricType::HyperboloidTwoSheets;
				steps.push_back("Nakon mnozenja s -1 dobivamo jedan pozitivan i dva negativna koeficijenta: hiperboloid dvije ljuske.");
			} else {
				type = QuadricType::Unknown;
				steps.push_back("Kombinacija predznaka ne odgovara standardnom obliku sredisnje kvadrike.");
			}
		}
	}

	return QuadricClassificationResult{type, nameFor(type), std::move(steps)};
}

QuadricClassificationResult Quadrics::classifyParaboloid(double a, double b, double e) {
	requireFinite(a, "A"); requireFinite(b, "B"); requireFinite(e, "E");
	if (a == 0.0 || b == 0.0) {
		throw std::invalid_argument("Koeficijenti A i B moraju biti razliciti od nule.");
	}
	if (e == 0.0) {
		throw std::invalid_argument("Koeficijent E mora biti razlicit od nule za paraboloid.");
	}

	std::vector<std::string> steps;
	steps.push_back("Kanonski oblik: A*x^2 + B*y^2 + E*z = 0 s A=" + std::to_string(a) + ", B=" + std::to_string(b) + ", E=" + std::to_string(e) + ".");

	QuadricType type;
	if ((a > 0 && b > 0) || (a < 0 && b < 0)) {
		type = QuadricType::EllipticParaboloid;
		steps.push_back("Koeficijenti A i B istog su predznaka: elipticki paraboloid.");
	} else {
		type = QuadricType::HyperbolicParaboloid;
		steps.push_back("Koeficijenti A i B suprotnog su predznaka: hiperbolicki paraboloid (sedlo).");
	}

	return QuadricClassificationResult{type, nameFor(type), std::move(steps)};
}

} // namespace aksiomat::linear_algebra
