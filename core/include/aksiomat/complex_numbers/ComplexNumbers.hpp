#pragma once

#include <string>
#include <vector>

namespace aksiomat::complex_numbers {

struct ComplexValue {
	double real;
	double imaginary;
};

struct ComplexOperationResult {
	ComplexValue result;
	std::vector<std::string> steps;
};

struct PolarFormResult {
	double modulus;
	double argument;
	double argumentDegrees;
	std::vector<std::string> steps;
};

struct AlgebraicFormResult {
	double real;
	double imaginary;
	std::vector<std::string> steps;
};

// Osnovne operacije nad kompleksnim brojevima i pretvorbe izmedu
// algebarskog (a + bi), trigonometrijskog (r(cos t + i sin t)) i
// eksponencijalnog (r * e^(i t)) oblika.
class ComplexNumbers {
public:
	static ComplexOperationResult add(ComplexValue a, ComplexValue b);
	static ComplexOperationResult subtract(ComplexValue a, ComplexValue b);
	static ComplexOperationResult multiply(ComplexValue a, ComplexValue b);
	static ComplexOperationResult divide(ComplexValue a, ComplexValue b);
	static ComplexOperationResult conjugate(ComplexValue a);

	static PolarFormResult toPolarForm(ComplexValue a);
	static AlgebraicFormResult toAlgebraicForm(double modulus, double argumentDegrees);
};

} // namespace aksiomat::complex_numbers
