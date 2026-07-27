#include "aksiomat/Arithmetic.hpp"

#include <cctype>
#include <cmath>
#include <stdexcept>

namespace aksiomat {

namespace {

// Rekurzivni parser:
//   expr   := term (('+' | '-') term)*
//   term   := factor (('*' | '/') factor)*
//   factor := number | '(' expr ')' | '-' factor
class Parser {
public:
	explicit Parser(const std::string& input) : input_(input) {}

	double parse() {
		double result = parseExpr();
		skipWhitespace();
		if (pos_ != input_.size()) {
			throw std::invalid_argument("Neocekivan znak na poziciji " + std::to_string(pos_));
		}
		return result;
	}

private:
	double parseExpr() {
		double value = parseTerm();
		while (true) {
			skipWhitespace();
			if (match('+')) {
				value += parseTerm();
			} else if (match('-')) {
				value -= parseTerm();
			} else {
				return value;
			}
		}
	}

	double parseTerm() {
		double value = parseFactor();
		while (true) {
			skipWhitespace();
			if (match('*')) {
				value *= parseFactor();
			} else if (match('/')) {
				double divisor = parseFactor();
				if (divisor == 0.0) {
					throw std::invalid_argument("Dijeljenje s nulom");
				}
				value /= divisor;
			} else {
				return value;
			}
		}
	}

	double parseFactor() {
		skipWhitespace();
		if (match('-')) {
			return -parseFactor();
		}
		if (match('(')) {
			double value = parseExpr();
			skipWhitespace();
			if (!match(')')) {
				throw std::invalid_argument("Nedostaje zatvorena zagrada");
			}
			return value;
		}
		return parseNumber();
	}

	double parseNumber() {
		skipWhitespace();
		std::size_t start = pos_;
		while (pos_ < input_.size() && (std::isdigit(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '.')) {
			++pos_;
		}
		if (start == pos_) {
			throw std::invalid_argument("Ocekivan broj na poziciji " + std::to_string(start));
		}
		try {
			return std::stod(input_.substr(start, pos_ - start));
		} catch (const std::exception&) {
			throw std::invalid_argument("Neispravan broj na poziciji " + std::to_string(start));
		}
	}

	bool match(char c) {
		if (pos_ < input_.size() && input_[pos_] == c) {
			++pos_;
			return true;
		}
		return false;
	}

	void skipWhitespace() {
		while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
			++pos_;
		}
	}

	const std::string& input_;
	std::size_t pos_ = 0;
};

} // namespace

double Arithmetic::evaluate(const std::string& expression) {
	return Parser(expression).parse();
}

} // namespace aksiomat
