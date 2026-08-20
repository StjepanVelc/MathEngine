#include "aksiomat/algebra/AlgebraParser.hpp"

#include <cctype>
#include <stdexcept>

namespace aksiomat::algebra {
namespace {

class Parser {
public:
	explicit Parser(const std::string& input) : input_(input) {}

	AlgebraExpression::Ptr run() {
		auto expression = parseExpression();
		skipWhitespace();
		if (pos_ != input_.size()) fail("Neocekivan znak");
		return expression;
	}

private:
	AlgebraExpression::Ptr parseExpression() {
		auto lhs = parseTerm();
		while (true) {
			if (match('+')) {
				lhs = AlgebraExpression::binary(AlgebraOp::Add, lhs, parseTerm());
			} else if (match('-')) {
				lhs = AlgebraExpression::binary(AlgebraOp::Subtract, lhs, parseTerm());
			} else {
				return lhs;
			}
		}
	}

	AlgebraExpression::Ptr parseTerm() {
		auto lhs = parseUnary();
		while (true) {
			if (match('*')) {
				lhs = AlgebraExpression::binary(AlgebraOp::Multiply, lhs, parseUnary());
			} else if (match('/')) {
				lhs = AlgebraExpression::binary(AlgebraOp::Divide, lhs, parseUnary());
			} else if (startsPrimary()) {
				lhs = AlgebraExpression::binary(AlgebraOp::Multiply, lhs, parseUnary());
			} else {
				return lhs;
			}
		}
	}

	AlgebraExpression::Ptr parseUnary() {
		if (match('+')) return AlgebraExpression::unary(AlgebraOp::Positive, parseUnary());
		if (match('-')) return AlgebraExpression::unary(AlgebraOp::Negate, parseUnary());
		return parsePower();
	}

	AlgebraExpression::Ptr parsePower() {
		auto lhs = parsePrimary();
		if (match('^')) return AlgebraExpression::binary(AlgebraOp::Power, lhs, parseUnary());
		return lhs;
	}

	AlgebraExpression::Ptr parsePrimary() {
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
			return AlgebraExpression::variable(parseIdentifier());
		}
		fail("Ocekivan broj, varijabla ili '('");
	}

	AlgebraExpression::Ptr parseNumber() {
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
			const std::size_t exponentMarker = pos_++;
			if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) ++pos_;
			const std::size_t exponentStart = pos_;
			while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
			if (exponentStart == pos_) pos_ = exponentMarker;
		}
		try {
			return AlgebraExpression::number(std::stod(input_.substr(start, pos_ - start)));
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

	bool startsPrimary() {
		skipWhitespace();
		if (pos_ >= input_.size()) return false;
		const char token = input_[pos_];
		return token == '(' || token == '.' || std::isdigit(static_cast<unsigned char>(token)) ||
			std::isalpha(static_cast<unsigned char>(token)) || token == '_';
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

AlgebraExpression::Ptr AlgebraParser::parse(const std::string& expression) {
	return Parser(expression).run();
}

} // namespace aksiomat::algebra
