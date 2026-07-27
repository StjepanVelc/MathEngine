#include "aksiomat/LogicParser.hpp"

#include <cctype>
#include <stdexcept>

namespace aksiomat {

namespace {

// Rekurzivni parser:
//   iff     := implies ('↔' implies)*
//   implies := or ('→' implies)?          (desno-asocijativna)
//   or      := and ('∨' and)*
//   and     := unary ('∧' unary)*
//   unary   := '¬' unary | atom
//   atom    := variable | 'T' | 'F' | '1' | '0' | '(' iff ')'
class Parser {
public:
	explicit Parser(const std::string& input) : input_(input) {}

	LogicExpression::Ptr parse() {
		auto result = parseIff();
		skipWhitespace();
		if (pos_ != input_.size()) {
			throw std::invalid_argument("Neocekivan znak na poziciji " + std::to_string(pos_));
		}
		return result;
	}

private:
	LogicExpression::Ptr parseIff() {
		auto value = parseImplies();
		while (true) {
			skipWhitespace();
			if (match("\u2194") || match("<->")) {
				value = LogicExpression::equivalence(value, parseImplies());
			} else {
				return value;
			}
		}
	}

	LogicExpression::Ptr parseImplies() {
		auto value = parseOr();
		skipWhitespace();
		if (match("\u2192") || match("->")) {
			return LogicExpression::implication(value, parseImplies());
		}
		return value;
	}

	LogicExpression::Ptr parseOr() {
		auto value = parseAnd();
		while (true) {
			skipWhitespace();
			if (match("\u2228") || match("|")) {
				value = LogicExpression::disjunction(value, parseAnd());
			} else {
				return value;
			}
		}
	}

	LogicExpression::Ptr parseAnd() {
		auto value = parseUnary();
		while (true) {
			skipWhitespace();
			if (match("\u2227") || match("&")) {
				value = LogicExpression::conjunction(value, parseUnary());
			} else {
				return value;
			}
		}
	}

	LogicExpression::Ptr parseUnary() {
		skipWhitespace();
		if (match("\u00AC") || match("!")) {
			return LogicExpression::negation(parseUnary());
		}
		return parseAtom();
	}

	LogicExpression::Ptr parseAtom() {
		skipWhitespace();
		if (match("(")) {
			auto value = parseIff();
			skipWhitespace();
			if (!match(")")) {
				throw std::invalid_argument("Nedostaje zatvorena zagrada");
			}
			return value;
		}
		if (pos_ < input_.size() && (input_[pos_] == '1' || input_[pos_] == '0')) {
			return LogicExpression::constant(input_[pos_++] == '1');
		}
		std::string name = parseIdentifier();
		if (name == "T") {
			return LogicExpression::constant(true);
		}
		if (name == "F") {
			return LogicExpression::constant(false);
		}
		return LogicExpression::variable(name);
	}

	std::string parseIdentifier() {
		std::size_t start = pos_;
		while (pos_ < input_.size() &&
			   (std::isalnum(static_cast<unsigned char>(input_[pos_])) || input_[pos_] == '_')) {
			++pos_;
		}
		if (start == pos_) {
			throw std::invalid_argument("Ocekivana varijabla ili '(' na poziciji " + std::to_string(start));
		}
		return input_.substr(start, pos_ - start);
	}

	bool match(const std::string& token) {
		if (input_.compare(pos_, token.size(), token) == 0) {
			pos_ += token.size();
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

void collectInto(const LogicExpression::Ptr& expr, std::set<std::string>& out) {
	if (!expr) {
		return;
	}
	if (expr->op() == LogicOp::Variable) {
		out.insert(expr->name());
	}
	collectInto(expr->left(), out);
	collectInto(expr->right(), out);
}

} // namespace

LogicExpression::Ptr LogicParser::parse(const std::string& formula) {
	return Parser(formula).parse();
}

std::set<std::string> LogicParser::collectVariables(const LogicExpression::Ptr& expr) {
	std::set<std::string> variables;
	collectInto(expr, variables);
	return variables;
}

} // namespace aksiomat
