#pragma once

namespace aksiomat::exponential_logarithmic {

struct PowerResult {
	double base;
	double exponent;
	double value;
};

struct RootResult {
	double radicand;
	double degree;
	double value;
};

class PowersRoots {
public:
	static PowerResult power(double base, double exponent);
	static RootResult root(double radicand, double degree);
	static double nthRoot(double radicand, double degree);
	static double rationalPower(double base, long numerator, long denominator);
	static bool isPerfectSquare(long long value);
};

} // namespace aksiomat::exponential_logarithmic
