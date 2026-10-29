#include <gtest/gtest.h>

#include "aksiomat/integral_calculus/IntegralCalculusJson.hpp"

using aksiomat::integral_calculus::IntegralCalculusJson;

namespace {

bool has(const std::string& s, const std::string& part) { return s.find(part) != std::string::npos; }

bool balanced(const std::string& s) {
	int depth = 0;
	bool inString = false;
	for (std::size_t i = 0; i < s.size(); ++i) {
		const char c = s[i];
		if (inString) {
			if (c == '\\') ++i;
			else if (c == '"') inString = false;
			continue;
		}
		if (c == '"') inString = true;
		else if (c == '{' || c == '[') ++depth;
		else if (c == '}' || c == ']') --depth;
		if (depth < 0) return false;
	}
	return depth == 0 && !inString;
}

} // namespace

TEST(IntegralJson, RiemannAndIndefinite) {
	const auto r = IntegralCalculusJson::riemann("x^2", 0, 1, 10, "left");
	EXPECT_TRUE(balanced(r));
	EXPECT_TRUE(has(r, "\"method\":\"left\""));
	EXPECT_TRUE(has(r, "\"rectangles\":[{"));
	const auto i = IntegralCalculusJson::indefinite("x*exp(x)");
	EXPECT_TRUE(balanced(i));
	EXPECT_TRUE(has(i, "\"found\":true"));
	EXPECT_TRUE(has(i, "\"verified\":true"));
	EXPECT_TRUE(has(IntegralCalculusJson::indefinite("exp(x^2)"), "\"found\":false"));
}

TEST(IntegralJson, DefiniteAreaImproper) {
	const auto d = IntegralCalculusJson::definite("x^2", 0, 3);
	EXPECT_TRUE(balanced(d));
	EXPECT_TRUE(has(d, "\"value\":9"));
	EXPECT_TRUE(balanced(IntegralCalculusJson::areaBetween("x", "x^2", 0, 1)));
	const auto p = IntegralCalculusJson::improper("1/x^2", "upper", 1, 0);
	EXPECT_TRUE(has(p, "\"kind\":\"convergent\""));
	EXPECT_TRUE(has(IntegralCalculusJson::improper("1/x", "upper", 1, 0), "\"kind\":\"divergent\""));
	EXPECT_TRUE(has(IntegralCalculusJson::improper("", "ptest_infinity", 2, 0), "\"kind\":\"convergent\""));
	EXPECT_THROW(IntegralCalculusJson::improper("x", "nonsense", 0, 0), std::invalid_argument);
	EXPECT_TRUE(balanced(IntegralCalculusJson::meanValue("x^2", 0, 3)));
}

TEST(IntegralJson, OdeInductionSequenceSample) {
	const auto o = IntegralCalculusJson::ode("separable", "2*x", "y", true, 0, 1, 1, 0);
	EXPECT_TRUE(balanced(o));
	EXPECT_TRUE(has(o, "\"verified\":true"));
	EXPECT_TRUE(balanced(IntegralCalculusJson::ode("growth", "", "", true, 0.1, 1000, 10, 0)));
	EXPECT_TRUE(balanced(IntegralCalculusJson::ode("logistic", "", "", true, 0.5, 10, 20, 100)));
	EXPECT_THROW(IntegralCalculusJson::ode("zzz", "", "", false, 0, 0, 0, 0), std::invalid_argument);
	EXPECT_TRUE(has(IntegralCalculusJson::induction("sum", "n", "n*(n+1)/2", 1, 30), "\"stepVerified\":true"));
	EXPECT_TRUE(balanced(IntegralCalculusJson::induction("inequality", "n", "2^n", 1, 30)));
	EXPECT_TRUE(has(IntegralCalculusJson::sequence("1/n", 0.01, 30), "\"converges\":true"));
	const auto s = IntegralCalculusJson::sample("1/x", -1, 1, 3);
	EXPECT_TRUE(balanced(s));
	EXPECT_THROW(IntegralCalculusJson::sample("x", 1, 0, 10), std::invalid_argument);
}
