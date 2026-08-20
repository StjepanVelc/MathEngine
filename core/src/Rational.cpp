#include "aksiomat/Rational.hpp"

#include <charconv>
#include <compare>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace aksiomat {
namespace {

std::int64_t checked(std::int64_t lhs, std::int64_t rhs, char operation) {
#if defined(__SIZEOF_INT128__)
	__int128 value = operation == '+' ? static_cast<__int128>(lhs) + rhs
				   : operation == '-' ? static_cast<__int128>(lhs) - rhs
				   : static_cast<__int128>(lhs) * rhs;
	if (value < std::numeric_limits<std::int64_t>::min() ||
		value > std::numeric_limits<std::int64_t>::max()) {
		throw std::overflow_error("Razlomak prelazi 64-bitni raspon");
	}
	return static_cast<std::int64_t>(value);
#else
	if (operation == '+') {
		if ((rhs > 0 && lhs > std::numeric_limits<std::int64_t>::max() - rhs) ||
			(rhs < 0 && lhs < std::numeric_limits<std::int64_t>::min() - rhs))
			throw std::overflow_error("Razlomak prelazi 64-bitni raspon");
		return lhs + rhs;
	}
	if (operation == '-') return checked(lhs, -rhs, '+');
	if (lhs != 0 && (rhs == -1 && lhs == std::numeric_limits<std::int64_t>::min()))
		throw std::overflow_error("Razlomak prelazi 64-bitni raspon");
	if (lhs != 0 && std::abs(rhs) > std::numeric_limits<std::int64_t>::max() / std::abs(lhs))
		throw std::overflow_error("Razlomak prelazi 64-bitni raspon");
	return lhs * rhs;
#endif
}

std::int64_t parseInteger(const std::string& text) {
	std::int64_t value = 0;
	const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
	if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
		throw std::invalid_argument("Neispravan cijeli broj '" + text + "'");
	}
	return value;
}

} // namespace

Rational::Rational(std::int64_t numerator, std::int64_t denominator) {
	if (denominator == 0) throw std::invalid_argument("Nazivnik ne smije biti nula");
	if (denominator < 0) {
		if (numerator == std::numeric_limits<std::int64_t>::min() ||
			denominator == std::numeric_limits<std::int64_t>::min()) {
			throw std::overflow_error("Razlomak prelazi 64-bitni raspon");
		}
		numerator = -numerator;
		denominator = -denominator;
	}
	const auto divisor = std::gcd(numerator, denominator);
	numerator_ = numerator / divisor;
	denominator_ = denominator / divisor;
}

Rational Rational::parse(const std::string& text) {
	const auto slash = text.find('/');
	if (slash == std::string::npos) return Rational(parseInteger(text));
	if (text.find('/', slash + 1) != std::string::npos) {
		throw std::invalid_argument("Neispravan zapis razlomka");
	}
	return Rational(parseInteger(text.substr(0, slash)), parseInteger(text.substr(slash + 1)));
}

Rational Rational::fromDecimal(const std::string& text) {
	const auto dot = text.find('.');
	if (dot == std::string::npos) return Rational(parseInteger(text));
	if (text.find('.', dot + 1) != std::string::npos) throw std::invalid_argument("Neispravan decimalni broj");
	const bool negative = !text.empty() && text.front() == '-';
	const std::string integralText = text.substr(0, dot);
	const std::string fractional = text.substr(dot + 1);
	if (fractional.empty() || fractional.size() > 18) throw std::invalid_argument("Decimalni zapis ima previse znamenki");
	std::int64_t denominator = 1;
	for (std::size_t i = 0; i < fractional.size(); ++i) denominator = checked(denominator, 10, '*');
	const auto integral = integralText == "-" || integralText.empty() ? 0 : parseInteger(integralText);
	const auto fraction = parseInteger(fractional);
	auto numerator = checked(integral, denominator, '*');
	numerator = negative ? checked(numerator, fraction, '-') : checked(numerator, fraction, '+');
	return Rational(numerator, denominator);
}

double Rational::toDouble() const {
	return static_cast<double>(numerator_) / static_cast<double>(denominator_);
}

std::string Rational::toString() const {
	if (denominator_ == 1) return std::to_string(numerator_);
	return std::to_string(numerator_) + '/' + std::to_string(denominator_);
}

std::string Rational::toMixedString() const {
	const auto whole = numerator_ / denominator_;
	const auto remainder = numerator_ % denominator_;
	if (remainder == 0) return std::to_string(whole);
	if (whole == 0) return toString();
	return std::to_string(whole) + " " + std::to_string(remainder < 0 ? -remainder : remainder) +
		   '/' + std::to_string(denominator_);
}

Rational operator+(const Rational& lhs, const Rational& rhs) {
	return Rational(checked(checked(lhs.numerator_, rhs.denominator_, '*'),
							checked(rhs.numerator_, lhs.denominator_, '*'), '+'),
					checked(lhs.denominator_, rhs.denominator_, '*'));
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
	return Rational(checked(checked(lhs.numerator_, rhs.denominator_, '*'),
							checked(rhs.numerator_, lhs.denominator_, '*'), '-'),
					checked(lhs.denominator_, rhs.denominator_, '*'));
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
	return Rational(checked(lhs.numerator_, rhs.numerator_, '*'),
					checked(lhs.denominator_, rhs.denominator_, '*'));
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
	if (rhs.numerator_ == 0) throw std::invalid_argument("Dijeljenje razlomkom nula");
	return Rational(checked(lhs.numerator_, rhs.denominator_, '*'),
					checked(lhs.denominator_, rhs.numerator_, '*'));
}

auto operator<=>(const Rational& lhs, const Rational& rhs) {
	const auto left = static_cast<long double>(lhs.numerator_) * rhs.denominator_;
	const auto right = static_cast<long double>(rhs.numerator_) * lhs.denominator_;
	return left <=> right;
}

} // namespace aksiomat
