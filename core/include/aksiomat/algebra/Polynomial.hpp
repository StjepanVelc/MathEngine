#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "aksiomat/algebra/AlgebraExpression.hpp"

namespace aksiomat::algebra {

struct PolynomialPoint {
	double x;
	double y;
};

class Polynomial {
public:
	Polynomial() = default;
	explicit Polynomial(std::map<int, double, std::greater<int>> coefficients);

	static Polynomial parse(const std::string& expression, const std::string& variable = "x");
	static Polynomial fromExpression(const AlgebraExpression::Ptr& expression, const std::string& variable = "x");

	int degree() const;
	double coefficient(int degree) const;
	double evaluate(double value) const;
	Polynomial derivative() const;
	std::vector<double> realRoots() const;
	std::optional<double> discriminant() const;
	std::optional<PolynomialPoint> vertex() const;
	std::optional<std::string> factorizedForm(const std::string& variable = "x") const;
	std::string toString(const std::string& variable = "x") const;

	Polynomial operator+(const Polynomial& rhs) const;
	Polynomial operator-(const Polynomial& rhs) const;
	Polynomial operator*(const Polynomial& rhs) const;

private:
	std::map<int, double, std::greater<int>> coefficients_;
	void normalize();
};

} // namespace aksiomat::algebra
