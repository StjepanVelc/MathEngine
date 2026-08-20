#include "aksiomat/NumeralSystems.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace aksiomat {
namespace {

void validateBase(unsigned base) {
	if (base < 2 || base > 36) throw std::invalid_argument("Baza mora biti izmedu 2 i 36");
}

unsigned digitValue(char digit) {
	if (digit >= '0' && digit <= '9') return static_cast<unsigned>(digit - '0');
	if (digit >= 'A' && digit <= 'Z') return static_cast<unsigned>(digit - 'A' + 10);
	if (digit >= 'a' && digit <= 'z') return static_cast<unsigned>(digit - 'a' + 10);
	return 36;
}

} // namespace

std::int64_t NumeralSystems::parse(const std::string& text, unsigned base) {
	validateBase(base);
	if (text.empty()) throw std::invalid_argument("Broj ne smije biti prazan");
	std::size_t pos = 0;
	bool negative = false;
	if (text[pos] == '+' || text[pos] == '-') {
		negative = text[pos] == '-';
		if (++pos == text.size()) throw std::invalid_argument("Nedostaju znamenke");
	}
	std::uint64_t value = 0;
	const std::uint64_t limit = negative
								  ? static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1
								  : static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
	for (; pos < text.size(); ++pos) {
		const auto digit = digitValue(text[pos]);
		if (digit >= base) throw std::invalid_argument("Znamenka nije valjana za zadanu bazu");
		if (value > (limit - digit) / base) throw std::overflow_error("Broj prelazi 64-bitni raspon");
		value = value * base + digit;
	}
	if (negative && value == limit) return std::numeric_limits<std::int64_t>::min();
	return negative ? -static_cast<std::int64_t>(value) : static_cast<std::int64_t>(value);
}

std::string NumeralSystems::format(std::int64_t value, unsigned base) {
	validateBase(base);
	if (value == 0) return "0";
	constexpr char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const bool negative = value < 0;
	std::uint64_t magnitude = negative
								  ? static_cast<std::uint64_t>(-(value + 1)) + 1
								  : static_cast<std::uint64_t>(value);
	std::string result;
	while (magnitude > 0) {
		result.push_back(digits[magnitude % base]);
		magnitude /= base;
	}
	if (negative) result.push_back('-');
	std::reverse(result.begin(), result.end());
	return result;
}

std::string NumeralSystems::convert(const std::string& text, unsigned fromBase, unsigned toBase) {
	return format(parse(text, fromBase), toBase);
}

} // namespace aksiomat
