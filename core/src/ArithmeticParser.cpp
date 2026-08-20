#include "aksiomat/ArithmeticParser.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>

namespace aksiomat {
namespace {

class Parser {
public:
	explicit Parser(const std::string& input) : input_(input) {}

	ArithmeticExpression::Ptr run() {
		auto expression = parseExpression();
		skipWhitespace();
		if (pos_ != input_.size()) fail("Neocekivan znak");
		return expression;
	}

private:
	ArithmeticExpression::Ptr parseExpression() {
		auto lhs = parseTerm();
		while (true) {
			if (match('+')) {
				lhs = ArithmeticExpression::binary(ArithmeticOp::Add, lhs, parseTerm());
			} else if (match('-')) {
				lhs = ArithmeticExpression::binary(ArithmeticOp::Subtract, lhs, parseTerm());
			} else {
				return lhs;
			}
		}
	}

	ArithmeticExpression::Ptr parseTerm() {
		auto lhs = parseUnary();
		while (true) {
			if (match('*')) {
				lhs = ArithmeticExpression::binary(ArithmeticOp::Multiply, lhs, parseUnary());
			} else if (match('/')) {
				lhs = ArithmeticExpression::binary(ArithmeticOp::Divide, lhs, parseUnary());
			} else if (match('%')) {
				lhs = ArithmeticExpression::binary(ArithmeticOp::Modulo, lhs, parseUnary());
			} else {
				return lhs;
			}
		}
	}

	ArithmeticExpression::Ptr parseUnary() {
		if (match('+')) return ArithmeticExpression::unary(ArithmeticOp::Positive, parseUnary());
		if (match('-')) return ArithmeticExpression::unary(ArithmeticOp::Negate, parseUnary());
		return parsePower();
	}

	ArithmeticExpression::Ptr parsePower() {
		auto lhs = parsePostfix();
		if (match('^')) {
			return ArithmeticExpression::binary(ArithmeticOp::Power, lhs, parseUnary());
		}
		return lhs;
	}

	ArithmeticExpression::Ptr parsePostfix() {
		auto expression = parsePrimary();
		while (match('!')) {
			expression = ArithmeticExpression::unary(ArithmeticOp::Factorial, expression);
		}
		return expression;
	}

	ArithmeticExpression::Ptr parsePrimary() {
		if (match('(')) {
			auto expression = parseExpression();
			if (!match(')')) fail("Nedostaje zatvorena zagrada");
			return expression;
		}

		skipWhitespace();
		if (pos_ < input_.size() &&
			(std::isdigit(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '.')) {
			return parseNumber();
		}
		if (pos_ < input_.size() &&
			(std::isalpha(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '_')) {
			const auto name = parseIdentifier();
			if (!match('(')) return ArithmeticExpression::constant(name);
			std::vector<ArithmeticExpression::Ptr> arguments;
			if (!peek(')')) {
				arguments.push_back(parseExpression());
				while (match(',')) arguments.push_back(parseExpression());
			}
			if (!match(')')) fail("Nedostaje zatvorena zagrada funkcije");
			return ArithmeticExpression::function(name, std::move(arguments));
		}
		fail("Ocekivan broj, konstanta, funkcija ili '('");
	}

	ArithmeticExpression::Ptr parseNumber() {
		skipWhitespace();
		const std::size_t start = pos_;
		bool hasDigits = false;
		while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
			hasDigits = true;
			++pos_;
		}
		if (pos_ < input_.size() && input_[pos_] == '.') {
			++pos_;
			while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) {
				hasDigits = true;
				++pos_;
			}
		}
		if (!hasDigits) fail("Neispravan broj");
		if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
			++pos_;
			if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) ++pos_;
			const std::size_t exponentStart = pos_;
			while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
			if (exponentStart == pos_) fail("Neispravan eksponent");
		}
		try {
			return ArithmeticExpression::number(std::stod(input_.substr(start, pos_ - start)));
		} catch (const std::exception&) {
			fail("Neispravan broj");
		}
	}

	std::string parseIdentifier() {
		const std::size_t start = pos_;
		while (pos_ < input_.size() &&
			   (std::isalnum(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '_')) ++pos_;
		return input_.substr(start, pos_ - start);
	}

	bool peek(char token) {
		skipWhitespace();
		return pos_ < input_.size() && input_[pos_] == token;
	}

	bool match(char token) {
		skipWhitespace();
		if (pos_ >= input_.size() || input_[pos_] != token) return false;
		++pos_;
		return true;
	}

	void skipWhitespace() {
		while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) ++pos_;
	}

	[[noreturn]] void fail(const std::string& message) const {
		throw std::invalid_argument(message + " na poziciji " + std::to_string(pos_));
	}

	const std::string& input_;
	std::size_t pos_ = 0;
};

} // namespace

ArithmeticExpression::Ptr ArithmeticParser::parse(const std::string& expression) {
	return Parser(expression).run();
}

} // namespace aksiomat
