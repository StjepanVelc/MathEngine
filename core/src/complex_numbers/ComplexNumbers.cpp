#include "aksiomat/complex_numbers/ComplexNumbers.hpp"

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

ComplexOperationResult ComplexNumbers::add(ComplexValue a, ComplexValue b) {
	ComplexValue sum{a.real + b.real, a.imaginary + b.imaginary};
	std::vector<std::string> steps;
	steps.push_back("Zbrajamo realne i imaginarne dijelove zasebno.");
	steps.push_back("(" + formatComplex(a) + ") + (" + formatComplex(b) + ") = " + formatComplex(sum));
	return {sum, steps};
}

ComplexOperationResult ComplexNumbers::subtract(ComplexValue a, ComplexValue b) {
	ComplexValue difference{a.real - b.real, a.imaginary - b.imaginary};
	std::vector<std::string> steps;
	steps.push_back("Oduzimamo realne i imaginarne dijelove zasebno.");
	steps.push_back("(" + formatComplex(a) + ") - (" + formatComplex(b) + ") = " + formatComplex(difference));
	return {difference, steps};
}

ComplexOperationResult ComplexNumbers::multiply(ComplexValue a, ComplexValue b) {
	ComplexValue product{
		a.real * b.real - a.imaginary * b.imaginary,
		a.real * b.imaginary + a.imaginary * b.real
	};
	std::vector<std::string> steps;
	steps.push_back("Mnozimo po distributivnosti i koristimo i^2 = -1.");
	steps.push_back("Realni dio: " + formatNumber(a.real) + "*" + formatNumber(b.real) + " - " + formatNumber(a.imaginary) + "*" + formatNumber(b.imaginary) + " = " + formatNumber(product.real));
	steps.push_back("Imaginarni dio: " + formatNumber(a.real) + "*" + formatNumber(b.imaginary) + " + " + formatNumber(a.imaginary) + "*" + formatNumber(b.real) + " = " + formatNumber(product.imaginary));
	steps.push_back("(" + formatComplex(a) + ") * (" + formatComplex(b) + ") = " + formatComplex(product));
	return {product, steps};
}

ComplexOperationResult ComplexNumbers::divide(ComplexValue a, ComplexValue b) {
	const double denominator = b.real * b.real + b.imaginary * b.imaginary;
	if (denominator == 0.0) throw std::invalid_argument("Dijeljenje s nulom nije definirano.");

	ComplexValue quotient{
		(a.real * b.real + a.imaginary * b.imaginary) / denominator,
		(a.imaginary * b.real - a.real * b.imaginary) / denominator
	};
	std::vector<std::string> steps;
	steps.push_back("Mnozimo brojnik i nazivnik konjugatom nazivnika.");
	steps.push_back("Nazivnik postaje |b|^2 = " + formatNumber(denominator) + ".");
	steps.push_back("(" + formatComplex(a) + ") / (" + formatComplex(b) + ") = " + formatComplex(quotient));
	return {quotient, steps};
}

ComplexOperationResult ComplexNumbers::conjugate(ComplexValue a) {
	ComplexValue result{a.real, -a.imaginary};
	std::vector<std::string> steps;
	steps.push_back("Konjugat mijenja predznak imaginarnog dijela.");
	steps.push_back("konj(" + formatComplex(a) + ") = " + formatComplex(result));
	return {result, steps};
}

PolarFormResult ComplexNumbers::toPolarForm(ComplexValue a) {
	const double modulus = std::sqrt(a.real * a.real + a.imaginary * a.imaginary);
	const double argument = std::atan2(a.imaginary, a.real);
	const double argumentDegrees = argument * 180.0 / kPi;

	std::vector<std::string> steps;
	steps.push_back("Modul: r = sqrt(a^2 + b^2) = " + formatNumber(modulus) + ".");
	steps.push_back("Argument: t = atan2(b, a) = " + formatNumber(argument) + " rad = " + formatNumber(argumentDegrees) + " stupnjeva.");
	steps.push_back("Trigonometrijski oblik: " + formatNumber(modulus) + "(cos(" + formatNumber(argumentDegrees) + " deg) + i*sin(" + formatNumber(argumentDegrees) + " deg))");
	steps.push_back("Eksponencijalni oblik: " + formatNumber(modulus) + " * e^(i*" + formatNumber(argument) + ")");
	return {modulus, argument, argumentDegrees, steps};
}

AlgebraicFormResult ComplexNumbers::toAlgebraicForm(double modulus, double argumentDegrees) {
	if (modulus < 0.0) throw std::invalid_argument("Modul mora biti nenegativan.");

	const double argumentRadians = argumentDegrees * kPi / 180.0;
	const double real = modulus * std::cos(argumentRadians);
	const double imaginary = modulus * std::sin(argumentRadians);

	std::vector<std::string> steps;
	steps.push_back("Pretvaramo kut u radijane: " + formatNumber(argumentDegrees) + " deg = " + formatNumber(argumentRadians) + " rad.");
	steps.push_back("Realni dio: a = r*cos(t) = " + formatNumber(real) + ".");
	steps.push_back("Imaginarni dio: b = r*sin(t) = " + formatNumber(imaginary) + ".");
	steps.push_back("Algebarski oblik: " + formatComplex({real, imaginary}));
	return {real, imaginary, steps};
}

} // namespace aksiomat::complex_numbers
