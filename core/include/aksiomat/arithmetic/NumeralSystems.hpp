#pragma once

#include <cstdint>
#include <string>

namespace aksiomat {

class NumeralSystems {
public:
	static std::int64_t parse(const std::string& text, unsigned base);
	static std::string format(std::int64_t value, unsigned base);
	static std::string convert(const std::string& text, unsigned fromBase, unsigned toBase);
};

} // namespace aksiomat
