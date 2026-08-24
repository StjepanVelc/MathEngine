#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <stdexcept>

#include "aksiomat/trigonometry/TrigFunctions.hpp"

using namespace aksiomat::trigonometry;

TEST(TrigonometryFunctions, EvaluatesCharacteristicAnglesExactly) {
	const auto sine = TrigFunctions::evaluate(TrigFunction::Sine, 30.0);
	EXPECT_DOUBLE_EQ(sine.value, 0.5);
	EXPECT_EQ(sine.exact, "1/2");
	EXPECT_TRUE(sine.hasExactForm);

	const auto cosine = TrigFunctions::evaluate(TrigFunction::Cosine, 135.0);
	EXPECT_NEAR(cosine.value, -std::sqrt(2.0) / 2.0, 1e-12);
	EXPECT_EQ(cosine.exact, "-sqrt(2)/2");
}

TEST(TrigonometryFunctions, EvaluatesRadiansAndGeneralAngles) {
	const auto tangent = TrigFunctions::evaluate(TrigFunction::Tangent, std::numbers::pi / 4.0, AngleUnit::Radians);
	EXPECT_NEAR(tangent.value, 1.0, 1e-12);
	EXPECT_EQ(tangent.exact, "1");
	const auto general = TrigFunctions::evaluate(TrigFunction::Sine, 17.0);
	EXPECT_FALSE(general.hasExactForm);
	EXPECT_NEAR(general.value, std::sin(Angles::toRadians(17.0)), 1e-12);
}

TEST(TrigonometryFunctions, RejectsUndefinedTangent) {
	EXPECT_THROW(TrigFunctions::evaluate(TrigFunction::Tangent, 90.0), std::invalid_argument);
	EXPECT_THROW(TrigFunctions::evaluate(TrigFunction::Tangent, 270.0), std::invalid_argument);
}

TEST(TrigonometryFunctions, CalculatesPrincipalInverseValues) {
	EXPECT_NEAR(TrigFunctions::inverse(TrigFunction::Sine, 0.5), 30.0, 1e-12);
	EXPECT_NEAR(TrigFunctions::inverse(TrigFunction::Cosine, 0.0), 90.0, 1e-12);
	EXPECT_NEAR(TrigFunctions::inverse(TrigFunction::Tangent, 1.0, AngleUnit::Radians), std::numbers::pi / 4.0, 1e-12);
}

TEST(TrigonometryFunctions, ValidatesDomainsAndEnums) {
	EXPECT_THROW(TrigFunctions::inverse(TrigFunction::Sine, 1.1), std::invalid_argument);
	EXPECT_THROW(TrigFunctions::evaluate(static_cast<TrigFunction>(99), 30.0), std::invalid_argument);
	EXPECT_THROW(TrigFunctions::inverse(TrigFunction::Cosine, 0.0, static_cast<AngleUnit>(99)), std::invalid_argument);
}
