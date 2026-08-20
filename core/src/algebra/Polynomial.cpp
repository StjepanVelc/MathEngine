#include "aksiomat/algebra/Polynomial.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "aksiomat/algebra/AlgebraParser.hpp"

namespace aksiomat::algebra {
namespace {

constexpr double epsilon = 1e-12;

std::string number(double value) {
	if (std::abs(value) < epsilon) value = 0.0;
	std::ostringstream output;
	output << std::setprecision(12) << value;
	return output.str();
}

Polynomial constant(double value) {
	return Polynomial({{0, value}});
}

Polynomial power(Polynomial base, int exponent) {
	Polynomial result = constant(1.0);
	while (exponent > 0) {
		if (exponent % 2 != 0) result = result * base;
		exponent /= 2;
		if (exponent > 0) base = base * base;
	}
	return result;
}

} // namespace

Polynomial::Polynomial(std::map<int, double, std::greater<int>> coefficients)
	: coefficients_(std::move(coefficients)) {
	normalize();
}

Polynomial Polynomial::parse(const std::string& expression, const std::string& variable) {
	return fromExpression(AlgebraParser::parse(expression), variable);
}

Polynomial Polynomial::fromExpression(const AlgebraExpression::Ptr& expression, const std::string& variable) {
	if (!expression) throw std::invalid_argument("Nedostaje polinom");
	switch (expression->op()) {
	case AlgebraOp::Number: return constant(expression->value());
	case AlgebraOp::Variable:
		if (expression->name() != variable) throw std::invalid_argument("Ocekivana je samo varijabla " + variable);
		return Polynomial({{1, 1.0}});
	case AlgebraOp::Positive: return fromExpression(expression->lhs(), variable);
	case AlgebraOp::Negate: return constant(-1.0) * fromExpression(expression->lhs(), variable);
	case AlgebraOp::Add: return fromExpression(expression->lhs(), variable) + fromExpression(expression->rhs(), variable);
	case AlgebraOp::Subtract: return fromExpression(expression->lhs(), variable) - fromExpression(expression->rhs(), variable);
	case AlgebraOp::Multiply: return fromExpression(expression->lhs(), variable) * fromExpression(expression->rhs(), variable);
	case AlgebraOp::Divide: {
		const auto denominator = fromExpression(expression->rhs(), variable);
		if (denominator.degree() != 0) throw std::invalid_argument("Polinom se moze dijeliti samo konstantom");
		const double divisor = denominator.coefficient(0);
		if (std::abs(divisor) < epsilon) throw std::invalid_argument("Dijeljenje nulom");
		return constant(1.0 / divisor) * fromExpression(expression->lhs(), variable);
	}
	case AlgebraOp::Power:
		if (expression->rhs()->op() != AlgebraOp::Number || expression->rhs()->value() < 0.0 ||
			std::floor(expression->rhs()->value()) != expression->rhs()->value() || expression->rhs()->value() > 64.0) {
			throw std::invalid_argument("Eksponent polinoma mora biti cijeli broj od 0 do 64");
		}
		return power(fromExpression(expression->lhs(), variable), static_cast<int>(expression->rhs()->value()));
	}
	throw std::invalid_argument("Nepodrzan polinom");
}

int Polynomial::degree() const {
	return coefficients_.empty() ? 0 : coefficients_.begin()->first;
}

double Polynomial::coefficient(int degree) const {
	const auto found = coefficients_.find(degree);
	return found == coefficients_.end() ? 0.0 : found->second;
}

double Polynomial::evaluate(double value) const {
	double result = 0.0;
	for (const auto& [degree, coefficientValue] : coefficients_) {
		result += coefficientValue * std::pow(value, degree);
	}
	return result;
}

Polynomial Polynomial::derivative() const {
	std::map<int, double, std::greater<int>> result;
	for (const auto& [degree, coefficientValue] : coefficients_) {
		if (degree > 0) result[degree - 1] = degree * coefficientValue;
	}
	return Polynomial(std::move(result));
}

std::vector<double> Polynomial::realRoots() const {
	if (degree() == 0) return {};
	if (degree() == 1) return {-coefficient(0) / coefficient(1)};
	if (degree() != 2) throw std::invalid_argument("Analiza nultocaka trenutno podrzava polinome do drugog stupnja");
	const double a = coefficient(2);
	const double b = coefficient(1);
	const double c = coefficient(0);
	const double discriminant = b * b - 4.0 * a * c;
	if (discriminant < -epsilon) return {};
	if (std::abs(discriminant) < epsilon) return {-b / (2.0 * a)};
	const double root = std::sqrt(discriminant);
	std::vector<double> result{(-b - root) / (2.0 * a), (-b + root) / (2.0 * a)};
	std::sort(result.begin(), result.end());
	return result;
}

std::optional<double> Polynomial::discriminant() const {
	if (degree() != 2) return std::nullopt;
	const double a = coefficient(2);
	const double b = coefficient(1);
	const double c = coefficient(0);
	return b * b - 4.0 * a * c;
}

std::optional<PolynomialPoint> Polynomial::vertex() const {
	if (degree() != 2) return std::nullopt;
	const double x = -coefficient(1) / (2.0 * coefficient(2));
	return PolynomialPoint{x, evaluate(x)};
}

std::optional<std::string> Polynomial::factorizedForm(const std::string& variable) const {
	if (degree() == 0) return toString(variable);
	if (degree() > 2) return std::nullopt;
	const auto roots = realRoots();
	if (roots.empty()) return std::nullopt;
	std::string result;
	const double leading = coefficient(degree());
	if (std::abs(leading - 1.0) >= epsilon) {
		if (std::abs(leading + 1.0) < epsilon) result = "-";
		else result = number(leading);
	}
	auto factor = [&](double root) {
		if (std::abs(root) < epsilon) return variable;
		return "(" + variable + (root < 0.0 ? " + " : " - ") + number(std::abs(root)) + ")";
	};
	result += factor(roots.front());
	if (degree() == 2) result += factor(roots.size() == 1 ? roots.front() : roots.back());
	return result;
}

std::string Polynomial::toString(const std::string& variable) const {
	if (coefficients_.empty()) return "0";
	std::string result;
	for (const auto& [degree, coefficientValue] : coefficients_) {
		const bool negative = coefficientValue < 0.0;
		const double magnitude = std::abs(coefficientValue);
		if (!result.empty()) result += negative ? " - " : " + ";
		else if (negative) result = "-";
		if (degree == 0 || std::abs(magnitude - 1.0) >= epsilon) result += number(magnitude);
		if (degree > 0) {
			result += variable;
			if (degree > 1) result += "^" + std::to_string(degree);
		}
	}
	return result;
}

Polynomial Polynomial::operator+(const Polynomial& rhs) const {
	auto result = coefficients_;
	for (const auto& [degree, value] : rhs.coefficients_) result[degree] += value;
	return Polynomial(std::move(result));
}

Polynomial Polynomial::operator-(const Polynomial& rhs) const {
	auto result = coefficients_;
	for (const auto& [degree, value] : rhs.coefficients_) result[degree] -= value;
	return Polynomial(std::move(result));
}

Polynomial Polynomial::operator*(const Polynomial& rhs) const {
	std::map<int, double, std::greater<int>> result;
	for (const auto& [lhsDegree, lhsValue] : coefficients_) {
		for (const auto& [rhsDegree, rhsValue] : rhs.coefficients_) {
			result[lhsDegree + rhsDegree] += lhsValue * rhsValue;
		}
	}
	return Polynomial(std::move(result));
}

void Polynomial::normalize() {
	for (auto it = coefficients_.begin(); it != coefficients_.end();) {
		if (it->first < 0) throw std::invalid_argument("Stupanj polinoma ne moze biti negativan");
		if (std::abs(it->second) < epsilon) it = coefficients_.erase(it);
		else ++it;
	}
}

} // namespace aksiomat::algebra
