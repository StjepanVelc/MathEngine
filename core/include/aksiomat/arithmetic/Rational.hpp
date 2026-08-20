#pragma once

#include <cstdint>
#include <string>

namespace aksiomat {

class Rational {
public:
	Rational(std::int64_t numerator = 0, std::int64_t denominator = 1);

	static Rational parse(const std::string& text);
	static Rational fromDecimal(const std::string& text);

	std::int64_t numerator() const { return numerator_; }
	std::int64_t denominator() const { return denominator_; }
	double toDouble() const;
	std::string toString() const;
	std::string toMixedString() const;

	friend Rational operator+(const Rational& lhs, const Rational& rhs);
	friend Rational operator-(const Rational& lhs, const Rational& rhs);
	friend Rational operator*(const Rational& lhs, const Rational& rhs);
	friend Rational operator/(const Rational& lhs, const Rational& rhs);
	friend bool operator==(const Rational&, const Rational&) = default;
	friend auto operator<=>(const Rational& lhs, const Rational& rhs);

private:
	std::int64_t numerator_;
	std::int64_t denominator_;
};

} // namespace aksiomat
