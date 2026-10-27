#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/FunctionExpression.hpp"

using namespace aksiomat::differential_calculus;

TEST(FunctionParser, ParsesAndEvaluatesElementaryFunctions) {
	EXPECT_NEAR(evaluate(parseFunction("2x^2 + 3sin(x)"), 1.0), 2.0 + 3.0 * std::sin(1.0), 1e-12);
	EXPECT_NEAR(evaluate(parseFunction("e^x"), 2.0), std::exp(2.0), 1e-12);
	EXPECT_NEAR(evaluate(parseFunction("ln(x) + sqrt(x)"), 4.0), std::log(4.0) + 2.0, 1e-12);
	EXPECT_NEAR(evaluate(parseFunction("-x^2"), 3.0), -9.0, 1e-12);
	EXPECT_NEAR(evaluate(parseFunction("2^3^2"), 0.0), 512.0, 1e-9);
	EXPECT_NEAR(evaluate(parseFunction("(x+1)(x-1)"), 3.0), 8.0, 1e-12);
	EXPECT_NEAR(evaluate(parseFunction("cos(pi*x)"), 1.0), -1.0, 1e-12);
}

TEST(FunctionParser, UndefinedPointsGiveNaN) {
	EXPECT_TRUE(std::isnan(evaluate(parseFunction("sqrt(x)"), -1.0)));
	EXPECT_TRUE(std::isnan(evaluate(parseFunction("ln(x)"), -2.0)));
}

TEST(FunctionParser, RejectsInvalidInput) {
	EXPECT_THROW(parseFunction(""), std::invalid_argument);
	EXPECT_THROW(parseFunction("2+"), std::invalid_argument);
	EXPECT_THROW(parseFunction("sin x"), std::invalid_argument);
	EXPECT_THROW(parseFunction("y + 1"), std::invalid_argument);
	EXPECT_THROW(parseFunction("(1+2"), std::invalid_argument);
	EXPECT_THROW(parseFunction("1 $ 2"), std::invalid_argument);
}

TEST(FunctionParser, SupportsOtherVariableName) {
	EXPECT_NEAR(evaluate(parseFunction("t^2 + t", "t"), 2.0), 6.0, 1e-12);
	EXPECT_THROW(parseFunction("x + t", "t"), std::invalid_argument);
}

TEST(FunctionExpression, SimplifiesAndPrints) {
	EXPECT_EQ(toString(simplify(parseFunction("1*x + 0"))), "x");
	EXPECT_EQ(toString(simplify(parseFunction("x^1"))), "x");
	EXPECT_EQ(toString(simplify(parseFunction("2*3 + x"))), "6 + x");
	EXPECT_EQ(toString(parseFunction("(x+1)*(x-1)")), "(x + 1)*(x - 1)");
	EXPECT_EQ(toString(parseFunction("x^(1/2)")), "x^(1/2)");
}

TEST(Differentiation, PolynomialDerivativeIsSimplified) {
	EXPECT_EQ(Differentiation::differentiate("x^3 + 2x").derivative, "3*x^2 + 2");
	EXPECT_EQ(Differentiation::differentiate("x^2").derivative, "2*x");
	EXPECT_EQ(Differentiation::differentiate("5").derivative, "0");
}

TEST(Differentiation, ElementaryFunctionsAtPoints) {
	EXPECT_NEAR(Differentiation::derivativeAt("sin(x)*x^2", 1.0), std::cos(1.0) + 2.0 * std::sin(1.0), 1e-10);
	EXPECT_NEAR(Differentiation::derivativeAt("exp(2*x)", 0.0), 2.0, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("ln(x)", 2.0), 0.5, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("x^x", 1.0), 1.0, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("sqrt(x)", 4.0), 0.25, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("atan(x)", 1.0), 0.5, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("1/x", 2.0), -0.25, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("tan(x)", 0.0), 1.0, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("(x^2+1)/(x-3)", 5.0), (2 * 5.0 * 2.0 - 26.0) / 4.0, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("2^x", 3.0), 8.0 * std::log(2.0), 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("asin(x)", 0.5), 1.0 / std::sqrt(0.75), 1e-12);
}

TEST(Differentiation, HigherOrderDerivatives) {
	EXPECT_NEAR(Differentiation::derivativeAt("sin(x)", std::numbers::pi / 2.0, 2), -1.0, 1e-12);
	EXPECT_NEAR(Differentiation::derivativeAt("x^4", 2.0, 3), 48.0, 1e-9);
	const auto result = Differentiation::differentiate("x^4", 2);
	EXPECT_EQ(result.derivative, "12*x^2");
	EXPECT_EQ(result.order, 2);
}

TEST(Differentiation, ProducesExplainedSteps) {
	const auto result = Differentiation::differentiate("sin(x^2)*x");
	ASSERT_GE(result.steps.size(), 3u);
	bool hasProduct = false;
	bool hasChain = false;
	for (const auto& step : result.steps) {
		if (step.find("pravilo produkta") != std::string::npos) hasProduct = true;
		if (step.find("lancano pravilo") != std::string::npos) hasChain = true;
	}
	EXPECT_TRUE(hasProduct);
	EXPECT_TRUE(hasChain);
}

TEST(Differentiation, RejectsInvalidOrder) {
	EXPECT_THROW(Differentiation::differentiate("x", 0), std::invalid_argument);
	EXPECT_THROW(Differentiation::differentiate("x", 11), std::invalid_argument);
}

TEST(Differentiation, EquivalenceCheck) {
	EXPECT_TRUE(equivalentFunctions("2*sin(x)*cos(x)", "sin(2*x)"));
	EXPECT_TRUE(equivalentFunctions("(x+1)^2", "x^2 + 2x + 1"));
	EXPECT_FALSE(equivalentFunctions("x^2", "x^3"));
}
