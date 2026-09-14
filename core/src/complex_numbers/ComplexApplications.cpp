#include "aksiomat/complex_numbers/ComplexApplications.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::complex_numbers {

namespace {

constexpr double kPi = 3.14159265358979323846;

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

std::string formatComplex(ComplexValue value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value.real;
	if (value.imaginary >= 0.0) stream << " + " << value.imaginary << "i";
	else stream << " - " << -value.imaginary << "i";
	return stream.str();
}

} // namespace

PowerResult ComplexApplications::power(ComplexValue base, int exponent) {
	const double modulus = std::sqrt(base.real * base.real + base.imaginary * base.imaginary);
	const double argument = std::atan2(base.imaginary, base.real);

	const double resultModulus = std::pow(modulus, exponent);
	const double resultArgument = argument * exponent;
	ComplexValue result{resultModulus * std::cos(resultArgument), resultModulus * std::sin(resultArgument)};

	std::vector<std::string> steps;
	steps.push_back("Pretvaramo u trigonometrijski oblik: r = " + formatNumber(modulus) + ", t = " + formatNumber(argument) + " rad.");
	steps.push_back("De Moivreova formula: z^n = r^n * (cos(n*t) + i*sin(n*t)).");
	steps.push_back("r^n = " + formatNumber(modulus) + "^" + std::to_string(exponent) + " = " + formatNumber(resultModulus) + ".");
	steps.push_back("n*t = " + formatNumber(resultArgument) + " rad.");
	steps.push_back("Rezultat: " + formatComplex(result));
	return {result, steps};
}

RootsResult ComplexApplications::nthRoots(ComplexValue value, int degree) {
	if (degree <= 0) throw std::invalid_argument("Stupanj korijena mora biti pozitivan cijeli broj.");

	const double modulus = std::sqrt(value.real * value.real + value.imaginary * value.imaginary);
	const double argument = std::atan2(value.imaginary, value.real);
	const double rootModulus = std::pow(modulus, 1.0 / degree);

	std::vector<ComplexValue> roots;
	std::vector<std::string> steps;
	steps.push_back("Pretvaramo u trigonometrijski oblik: r = " + formatNumber(modulus) + ", t = " + formatNumber(argument) + " rad.");
	steps.push_back("Modul svakog korijena: r^(1/n) = " + formatNumber(rootModulus) + ".");
	for (int k = 0; k < degree; ++k) {
		const double rootArgument = (argument + 2.0 * kPi * k) / degree;
		ComplexValue root{rootModulus * std::cos(rootArgument), rootModulus * std::sin(rootArgument)};
		roots.push_back(root);
		steps.push_back("k=" + std::to_string(k) + ": kut = " + formatNumber(rootArgument) + " rad, korijen = " + formatComplex(root));
	}
	return {roots, steps};
}

QuadraticEquationResult ComplexApplications::solveQuadraticEquation(double a, double b, double c) {
	if (a == 0.0) throw std::invalid_argument("Koeficijent uz kvadratni clan ne smije biti nula.");

	const double discriminant = b * b - 4.0 * a * c;
	std::vector<std::string> steps;
	steps.push_back("Diskriminanta: D = b^2 - 4ac = " + formatNumber(discriminant) + ".");

	ComplexValue root1, root2;
	if (discriminant >= 0.0) {
		const double sqrtDiscriminant = std::sqrt(discriminant);
		root1 = {(-b + sqrtDiscriminant) / (2.0 * a), 0.0};
		root2 = {(-b - sqrtDiscriminant) / (2.0 * a), 0.0};
		steps.push_back("D >= 0, rjesenja su realna.");
	} else {
		const double sqrtDiscriminant = std::sqrt(-discriminant);
		root1 = {-b / (2.0 * a), sqrtDiscriminant / (2.0 * a)};
		root2 = {-b / (2.0 * a), -sqrtDiscriminant / (2.0 * a)};
		steps.push_back("D < 0, rjesenja su konjugirano kompleksna.");
	}
	steps.push_back("x1 = " + formatComplex(root1));
	steps.push_back("x2 = " + formatComplex(root2));
	return {root1, root2, steps};
}

} // namespace aksiomat::complex_numbers
