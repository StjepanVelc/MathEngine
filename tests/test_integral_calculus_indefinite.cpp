#include <gtest/gtest.h>

#include "aksiomat/integral_calculus/IndefiniteIntegral.hpp"

using aksiomat::integral_calculus::IndefiniteIntegration;

namespace {

void expectFound(const std::string& f) {
	const auto r = IndefiniteIntegration::integrate(f);
	EXPECT_TRUE(r.found) << f << " : " << r.message;
	if (r.found) EXPECT_TRUE(r.verified) << f << " -> " << r.antiderivative << " ; " << r.verification;
}

} // namespace

TEST(IndefiniteIntegral, PolynomialsAndLinearity) {
	expectFound("3x^2 + 2x - 5");
	expectFound("x^3 - 4/x^0 + 7");
	expectFound("(x+1)^2");
	expectFound("5");
}

TEST(IndefiniteIntegral, TableFunctions) {
	for (const char* f : {"sin(x)", "cos(x)", "exp(x)", "1/x", "tan(x)", "1/sqrt(x)", "sqrt(x)", "2^x",
			"sin(2x+1)", "exp(3x)", "1/(2x+1)", "1/(3x-2)^2", "ln(x)", "atan(x)", "asin(x)",
			"1/cos(x)^2", "sin(x)^2", "cos(x)^3", "sinh(x)", "cosh(x)"})
		expectFound(f);
}

TEST(IndefiniteIntegral, Substitution) {
	for (const char* f : {"2x*exp(x^2)", "cos(x)*exp(sin(x))", "x*sqrt(x^2+1)", "(2x+3)*(x^2+3x)^4",
			"sin(x)*cos(x)", "cos(x)/sin(x)", "ln(x)/x", "x/(x^2+1)"})
		expectFound(f);
}

TEST(IndefiniteIntegral, ByParts) {
	for (const char* f : {"x*exp(x)", "x*sin(x)", "x*cos(x)", "x^2*exp(x)", "x*ln(x)", "x^2*ln(x)", "exp(x)*sin(x)",
			"exp(x)*cos(x)", "x*atan(x)"})
		expectFound(f);
}

TEST(IndefiniteIntegral, Rational) {
	for (const char* f : {"1/(x^2-1)", "1/(x^2+1)", "1/(x^2+2x+5)", "(x+1)/(x^2-3x+2)", "x^2/(x+1)",
			"1/(x^2*(x+1))", "(3x+2)/(x^2+4)", "1/(x^3-x)", "1/(x^2-2x+1)", "x^3/(x^2+1)"})
		expectFound(f);
}

TEST(IndefiniteIntegral, StepsAndFormat) {
	const auto r = IndefiniteIntegration::integrate("x*exp(x)");
	ASSERT_TRUE(r.found);
	EXPECT_EQ(r.method, "parcijalna integracija");
	EXPECT_FALSE(r.steps.empty());
	EXPECT_NE(r.antiderivativeWithC.find("+ C"), std::string::npos);
}

TEST(IndefiniteIntegral, UnsupportedIsHonest) {
	const auto r = IndefiniteIntegration::integrate("exp(x^2)");
	EXPECT_FALSE(r.found);
	EXPECT_FALSE(r.message.empty());
}

TEST(IndefiniteIntegral, InvalidInput) {
	EXPECT_THROW(IndefiniteIntegration::integrate("2 +* x"), std::invalid_argument);
}

TEST(IndefiniteIntegral, IsAntiderivative) {
	EXPECT_TRUE(IndefiniteIntegration::isAntiderivative("x^3/3", "x^2"));
	EXPECT_FALSE(IndefiniteIntegration::isAntiderivative("x^3", "x^2"));
}
