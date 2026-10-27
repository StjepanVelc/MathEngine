#include "aksiomat/differential_calculus/FunctionExpression.hpp"

#include <cctype>
#include <numbers>
#include <stdexcept>

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;

constexpr std::size_t kMaxLength = 500;
constexpr int kMaxDepth = 150;

class Parser {
public:
	Parser(const std::string& input, const std::string& variable) : input_(input), variable_(variable) {}

	Ptr run() {
		if (input_.size() > kMaxLength) fail("Izraz je predug");
		const Ptr expression = parseExpression();
		skipWhitespace();
		if (pos_ != input_.size()) fail("Neocekivan znak '" + std::string(1, input_[pos_]) + "'");
		return expression;
	}

private:
	struct DepthGuard {
		explicit DepthGuard(Parser& parser) : parser_(parser) {
			if (++parser_.depth_ > kMaxDepth) parser_.fail("Izraz je prekompliciran");
		}
		~DepthGuard() { --parser_.depth_; }
		Parser& parser_;
	};

	Ptr parseExpression() {
		DepthGuard guard(*this);
		Ptr lhs = parseTerm();
		while (true) {
			if (match('+')) {
				lhs = FnExpr::binary(FnOp::Add, lhs, parseTerm());
			} else if (match('-')) {
				lhs = FnExpr::binary(FnOp::Subtract, lhs, parseTerm());
			} else {
				return lhs;
			}
		}
	}

	Ptr parseTerm() {
		Ptr lhs = parseUnary();
		while (true) {
			if (match('*')) {
				lhs = FnExpr::binary(FnOp::Multiply, lhs, parseUnary());
			} else if (match('/')) {
				lhs = FnExpr::binary(FnOp::Divide, lhs, parseUnary());
			} else if (startsPrimary()) {
				lhs = FnExpr::binary(FnOp::Multiply, lhs, parseUnary());
			} else {
				return lhs;
			}
		}
	}

	Ptr parseUnary() {
		DepthGuard guard(*this);
		if (match('+')) return parseUnary();
		if (match('-')) return FnExpr::unary(FnOp::Negate, parseUnary());
		return parsePower();
	}

	Ptr parsePower() {
		Ptr base = parsePrimary();
		if (match('^')) return FnExpr::binary(FnOp::Power, base, parseUnary());
		return base;
	}

	Ptr parsePrimary() {
		if (match('(')) {
			Ptr inner = parseExpression();
			if (!match(')')) fail("Nedostaje zatvorena zagrada");
			return inner;
		}
		skipWhitespace();
		if (pos_ < input_.size() && (std::isdigit(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '.')) {
			return parseNumber();
		}
		if (pos_ < input_.size() && std::isalpha(static_cast<unsigned char>(input_[pos_]))) {
			return parseIdentifier();
		}
		fail("Ocekivan broj, varijabla, funkcija ili '('");
	}

	Ptr parseNumber() {
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
			const std::size_t marker = pos_++;
			if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) ++pos_;
			const std::size_t exponentStart = pos_;
			while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
			if (exponentStart == pos_) pos_ = marker;
		}
		try {
			return FnExpr::number(std::stod(input_.substr(start, pos_ - start)));
		} catch (const std::exception&) {
			fail("Neispravan broj");
		}
	}

	Ptr parseIdentifier() {
		const std::size_t start = pos_;
		while (pos_ < input_.size() && std::isalpha(static_cast<unsigned char>(input_[pos_]))) ++pos_;
		const std::string name = input_.substr(start, pos_ - start);

		struct Entry { const char* name; FnFunc func; };
		static const Entry functions[] = {
			{"sin", FnFunc::Sin}, {"cos", FnFunc::Cos}, {"tan", FnFunc::Tan}, {"tg", FnFunc::Tan},
			{"exp", FnFunc::Exp}, {"ln", FnFunc::Ln}, {"log", FnFunc::Ln}, {"sqrt", FnFunc::Sqrt},
			{"abs", FnFunc::Abs}, {"asin", FnFunc::Asin}, {"acos", FnFunc::Acos}, {"atan", FnFunc::Atan},
			{"arctan", FnFunc::Atan}, {"arcsin", FnFunc::Asin}, {"arccos", FnFunc::Acos},
			{"sinh", FnFunc::Sinh}, {"cosh", FnFunc::Cosh}, {"tanh", FnFunc::Tanh}
		};
		for (const Entry& entry : functions) {
			if (name == entry.name) {
				if (!match('(')) fail("Funkcija '" + name + "' trazi zagradu, npr. " + name + "(x)");
				Ptr argument = parseExpression();
				if (!match(')')) fail("Nedostaje zatvorena zagrada");
				return FnExpr::call(entry.func, argument);
			}
		}
		if (name == variable_) return FnExpr::variable(variable_);
		if (name == "pi") return FnExpr::number(std::numbers::pi);
		if (name == "e") return FnExpr::number(std::numbers::e);
		fail("Nepoznata oznaka '" + name + "'");
	}

	bool startsPrimary() {
		skipWhitespace();
		if (pos_ >= input_.size()) return false;
		const char token = input_[pos_];
		return token == '(' || token == '.' || std::isdigit(static_cast<unsigned char>(token)) ||
			std::isalpha(static_cast<unsigned char>(token));
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
	std::string variable_;
	std::size_t pos_ = 0;
	int depth_ = 0;
};

} // namespace

FnExpr::Ptr parseFunction(const std::string& text, const std::string& variable) {
	if (variable.empty()) throw std::invalid_argument("Naziv varijable ne smije biti prazan");
	if (text.find_first_not_of(" \t\r\n") == std::string::npos) throw std::invalid_argument("Izraz je prazan");
	return Parser(text, variable).run();
}

} // namespace aksiomat::differential_calculus
