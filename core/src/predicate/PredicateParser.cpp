#include "aksiomat/predicate/PredicateParser.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>
#include <vector>

namespace aksiomat {
namespace {

class Parser {
public:
	explicit Parser(const std::string& text) : text_(text) {}

	PredicateExpression::Ptr run() {
		auto expression = parseIff();
		skipWhitespace();
		if (pos_ != text_.size()) fail("Neocekivani znak");
		return expression;
	}

private:
	PredicateExpression::Ptr parseIff() {
		auto lhs = parseImplies();
		while (match("\u2194") || match("<->")) {
			lhs = PredicateExpression::equivalence(lhs, parseImplies());
		}
		return lhs;
	}

	PredicateExpression::Ptr parseImplies() {
		auto lhs = parseOr();
		if (match("\u2192") || match("->")) {
			return PredicateExpression::implication(lhs, parseImplies());
		}
		return lhs;
	}

	PredicateExpression::Ptr parseOr() {
		auto lhs = parseAnd();
		while (match("\u2228") || match("|")) {
			lhs = PredicateExpression::disjunction(lhs, parseAnd());
		}
		return lhs;
	}

	PredicateExpression::Ptr parseAnd() {
		auto lhs = parseUnary();
		while (match("\u2227") || match("&")) {
			lhs = PredicateExpression::conjunction(lhs, parseUnary());
		}
		return lhs;
	}

	PredicateExpression::Ptr parseUnary() {
		if (match("\u00AC") || match("!")) return PredicateExpression::negation(parseUnary());
		if (match("\u2200") || matchKeyword("forall")) {
			auto variable = parseIdentifier("Ocekivano ime varijable iza \u2200");
			auto body = parseUnary();
			return PredicateExpression::forAll(std::move(variable), std::move(body));
		}
		if (match("\u2203") || matchKeyword("exists")) {
			auto variable = parseIdentifier("Ocekivano ime varijable iza \u2203");
			auto body = parseUnary();
			return PredicateExpression::exists(std::move(variable), std::move(body));
		}
		return parseAtom();
	}

	PredicateExpression::Ptr parseAtom() {
		if (match("(")) {
			auto expression = parseIff();
			if (!match(")")) fail("Ocekivana ')'");
			return expression;
		}
		const auto name = parseIdentifier("Ocekivan predikat, term ili '('");
		if (match("!=" ) || match("\u2260")) {
			return PredicateExpression::notEqual(name,
				parseIdentifier("Ocekivan desni term nejednakosti"));
		}
		if (match("=")) {
			return PredicateExpression::equality(name,
				parseIdentifier("Ocekivan desni term jednakosti"));
		}
		if (!match("(")) {
			fail("Ocekivana '(', '=' ili '\u2260' iza identifikatora '" + name + "'");
		}
		std::vector<std::string> arguments;
		arguments.push_back(parseIdentifier("Ocekivan argument predikata"));
		while (match(",")) arguments.push_back(parseIdentifier("Ocekivan argument predikata"));
		if (!match(")")) fail("Ocekivana ')' iza argumenata predikata");
		return PredicateExpression::predicate(name, std::move(arguments));
	}

	std::string parseIdentifier(const std::string& message) {
		skipWhitespace();
		const std::size_t start = pos_;
		while (pos_ < text_.size() &&
			   (std::isalnum(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '_')) {
			++pos_;
		}
		if (start == pos_) fail(message);
		return text_.substr(start, pos_ - start);
	}

	bool matchKeyword(const std::string& keyword) {
		skipWhitespace();
		if (text_.compare(pos_, keyword.size(), keyword) != 0) return false;
		const std::size_t next = pos_ + keyword.size();
		if (next < text_.size() &&
			(std::isalnum(static_cast<unsigned char>(text_[next])) || text_[next] == '_')) {
			return false;
		}
		pos_ = next;
		return true;
	}

	bool match(const std::string& token) {
		skipWhitespace();
		if (text_.compare(pos_, token.size(), token) != 0) return false;
		pos_ += token.size();
		return true;
	}

	void skipWhitespace() {
		while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) ++pos_;
	}

	[[noreturn]] void fail(const std::string& message) const {
		throw std::invalid_argument(message + " na poziciji " + std::to_string(pos_));
	}

	const std::string& text_;
	std::size_t pos_ = 0;
};

void collectFreeInto(const PredicateExpression::Ptr& expression,
					 std::set<std::string>& bound, std::set<std::string>& free) {
	switch (expression->op()) {
	case PredicateOp::Predicate:
	case PredicateOp::Equality:
	case PredicateOp::NotEqual:
		for (const auto& argument : expression->arguments()) {
			if (!bound.contains(argument)) free.insert(argument);
		}
		return;
	case PredicateOp::Not:
		collectFreeInto(expression->left(), bound, free);
		return;
	case PredicateOp::ForAll:
	case PredicateOp::Exists: {
		const bool alreadyBound = bound.contains(expression->name());
		bound.insert(expression->name());
		collectFreeInto(expression->left(), bound, free);
		if (!alreadyBound) bound.erase(expression->name());
		return;
	}
	default:
		collectFreeInto(expression->left(), bound, free);
		collectFreeInto(expression->right(), bound, free);
	}
}

void collectBoundInto(const PredicateExpression::Ptr& expression, std::set<std::string>& bound) {
	switch (expression->op()) {
	case PredicateOp::Predicate:
	case PredicateOp::Equality:
	case PredicateOp::NotEqual:
		return;
	case PredicateOp::Not:
		collectBoundInto(expression->left(), bound);
		return;
	case PredicateOp::ForAll:
	case PredicateOp::Exists:
		bound.insert(expression->name());
		collectBoundInto(expression->left(), bound);
		return;
	default:
		collectBoundInto(expression->left(), bound);
		collectBoundInto(expression->right(), bound);
	}
}

void collectPredicatesInto(const PredicateExpression::Ptr& expression,
						   std::map<std::string, std::size_t>& predicates) {
	switch (expression->op()) {
	case PredicateOp::Predicate: {
		const auto arity = expression->arguments().size();
		const auto [it, inserted] = predicates.emplace(expression->name(), arity);
		if (!inserted && it->second != arity) {
			throw std::invalid_argument("Predikat '" + expression->name() +
										"' koristi se s razlicitim arnostima (" +
										std::to_string(it->second) + " i " +
										std::to_string(arity) + ")");
		}
		return;
	}
	case PredicateOp::Equality:
	case PredicateOp::NotEqual:
		return;
	case PredicateOp::Not:
	case PredicateOp::ForAll:
	case PredicateOp::Exists:
		collectPredicatesInto(expression->left(), predicates);
		return;
	default:
		collectPredicatesInto(expression->left(), predicates);
		collectPredicatesInto(expression->right(), predicates);
	}
}

} // namespace

PredicateExpression::Ptr PredicateParser::parse(const std::string& formula) {
	auto expression = Parser(formula).run();
	collectPredicates(expression);
	return expression;
}

std::set<std::string> PredicateParser::collectFreeVariables(const PredicateExpression::Ptr& expression) {
	std::set<std::string> bound;
	std::set<std::string> free;
	collectFreeInto(expression, bound, free);
	return free;
}

std::set<std::string> PredicateParser::collectBoundVariables(const PredicateExpression::Ptr& expression) {
	std::set<std::string> bound;
	collectBoundInto(expression, bound);
	return bound;
}

std::map<std::string, std::size_t> PredicateParser::collectPredicates(
	const PredicateExpression::Ptr& expression) {
	std::map<std::string, std::size_t> predicates;
	collectPredicatesInto(expression, predicates);
	return predicates;
}

} // namespace aksiomat
