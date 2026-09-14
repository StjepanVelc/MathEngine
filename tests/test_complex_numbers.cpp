#include "aksiomat/complex_numbers/ComplexNumbers.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::complex_numbers;

TEST(ComplexNumbers, AddsTwoComplexNumbers) {
	ComplexOperationResult result = ComplexNumbers::add({1, 2}, {3, -4});
	EXPECT_DOUBLE_EQ(result.result.real, 4.0);
	EXPECT_DOUBLE_EQ(result.result.imaginary, -2.0);
}

TEST(ComplexNumbers, SubtractsTwoComplexNumbers) {
	ComplexOperationResult result = ComplexNumbers::subtract({1, 2}, {3, -4});
	EXPECT_DOUBLE_EQ(result.result.real, -2.0);
	EXPECT_DOUBLE_EQ(result.result.imaginary, 6.0);
}

TEST(ComplexNumbers, MultipliesTwoComplexNumbers) {
	ComplexOperationResult result = ComplexNumbers::multiply({1, 2}, {3, 4});
	EXPECT_DOUBLE_EQ(result.result.real, -5.0);
	EXPECT_DOUBLE_EQ(result.result.imaginary, 10.0);
}

TEST(ComplexNumbers, DividesTwoComplexNumbers) {
	ComplexOperationResult result = ComplexNumbers::divide({1, 2}, {3, 4});
	EXPECT_NEAR(result.result.real, 0.44, 1e-9);
	EXPECT_NEAR(result.result.imaginary, 0.08, 1e-9);
}

TEST(ComplexNumbers, DivideRejectsZeroDenominator) {
	EXPECT_THROW(ComplexNumbers::divide({1, 2}, {0, 0}), std::invalid_argument);
}

TEST(ComplexNumbers, ComputesConjugate) {
	ComplexOperationResult result = ComplexNumbers::conjugate({3, -5});
	EXPECT_DOUBLE_EQ(result.result.real, 3.0);
	EXPECT_DOUBLE_EQ(result.result.imaginary, 5.0);
}

TEST(ComplexNumbers, ConvertsToPolarForm) {
	PolarFormResult result = ComplexNumbers::toPolarForm({0, 1});
	EXPECT_NEAR(result.modulus, 1.0, 1e-9);
	EXPECT_NEAR(result.argumentDegrees, 90.0, 1e-9);
}

TEST(ComplexNumbers, ConvertsToAlgebraicForm) {
	AlgebraicFormResult result = ComplexNumbers::toAlgebraicForm(1.0, 90.0);
	EXPECT_NEAR(result.real, 0.0, 1e-9);
	EXPECT_NEAR(result.imaginary, 1.0, 1e-9);
}

TEST(ComplexNumbers, ToAlgebraicFormRejectsNegativeModulus) {
	EXPECT_THROW(ComplexNumbers::toAlgebraicForm(-1.0, 0.0), std::invalid_argument);
}
