#include <gtest/gtest.h>

#include <stdexcept>

#include "aksiomat/differential_calculus/DifferentialCalculusJson.hpp"

using aksiomat::differential_calculus::DifferentialCalculusJson;

TEST(DifferentialCalculusJson, LimitSerializesInfinityKinds) {
	const auto json = DifferentialCalculusJson::limit("1/x", "0");
	EXPECT_NE(json.find("\"kind\":\"plus_infinity\""), std::string::npos);
	EXPECT_NE(json.find("\"kind\":\"minus_infinity\""), std::string::npos);
	EXPECT_NE(json.find("\"value\":null"), std::string::npos);
}

TEST(DifferentialCalculusJson, LimitAtInfinityAcceptsText) {
	const auto json = DifferentialCalculusJson::limit("1/x", "inf");
	EXPECT_NE(json.find("\"atInfinity\":true"), std::string::npos);
	EXPECT_THROW(DifferentialCalculusJson::limit("x", "abc"), std::invalid_argument);
}

TEST(DifferentialCalculusJson, OtherEndpointsProduceObjects) {
	EXPECT_EQ(DifferentialCalculusJson::derivative("x^2", 1).front(), '{');
	EXPECT_EQ(DifferentialCalculusJson::continuity("x^2", 1.0).front(), '{');
	EXPECT_EQ(DifferentialCalculusJson::tangent("x^2", 1.0).front(), '{');
	EXPECT_EQ(DifferentialCalculusJson::meanValue("x^2", 0.0, 2.0).front(), '{');
	EXPECT_EQ(DifferentialCalculusJson::study("x^3-3x", -3.0, 3.0).front(), '{');
	EXPECT_EQ(DifferentialCalculusJson::sample("ln(x)", -1.0, 1.0, 10).front(), '[');
}
