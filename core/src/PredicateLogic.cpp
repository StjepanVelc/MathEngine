#include "aksiomat/PredicateLogic.hpp"

#include <cctype>
#include <optional>
#include <stdexcept>

namespace aksiomat {

// ---------------------------------------------------------------------------
// PredicateExpression
// ---------------------------------------------------------------------------

PredicateExpression::PredicateExpression(PredicateOp op, std::string name,
										 std::vector<std::string> arguments, Ptr left, Ptr right)
	: op_(op), name_(std::move(name)), arguments_(std::move(arguments)),
	  left_(std::move(left)), right_(std::move(right)) {}

PredicateExpression::Ptr PredicateExpression::predicate(std::string name,
														std::vector<std::string> arguments) {
	return Ptr(new PredicateExpression(PredicateOp::Predicate, std::move(name),
									   std::move(arguments), nullptr, nullptr));
}

PredicateExpression::Ptr PredicateExpression::negation(Ptr operand) {
	return Ptr(new PredicateExpression(PredicateOp::Not, "", {}, std::move(operand), nullptr));
}

PredicateExpression::Ptr PredicateExpression::conjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::And, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::disjunction(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Or, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::implication(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Implies, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::equivalence(Ptr lhs, Ptr rhs) {
	return Ptr(new PredicateExpression(PredicateOp::Iff, "", {}, std::move(lhs), std::move(rhs)));
}

PredicateExpression::Ptr PredicateExpression::forAll(std::string variable, Ptr body) {
	return Ptr(new PredicateExpression(PredicateOp::ForAll, std::move(variable), {},
									   std::move(body), nullptr));
}

PredicateExpression::Ptr PredicateExpression::exists(std::string variable, Ptr body) {
	return Ptr(new PredicateExpression(PredicateOp::Exists, std::move(variable), {},
									   std::move(body), nullptr));
}

std::string PredicateExpression::toString() const {
	switch (op_) {
	case PredicateOp::Predicate: {
		std::string text = name_ + "(";
		for (std::size_t i = 0; i < arguments_.size(); ++i) {
			if (i > 0) text += ", ";
			text += arguments_[i];
		}
		return text + ")";
	}
	case PredicateOp::Not:
		return "\u00AC" + left_->toString();
	case PredicateOp::And:
		return "(" + left_->toString() + " \u2227 " + right_->toString() + ")";
	case PredicateOp::Or:
		return "(" + left_->toString() + " \u2228 " + right_->toString() + ")";
	case PredicateOp::Implies:
		return "(" + left_->toString() + " \u2192 " + right_->toString() + ")";
	case PredicateOp::Iff:
		return "(" + left_->toString() + " \u2194 " + right_->toString() + ")";
	case PredicateOp::ForAll:
		return "\u2200" + name_ + " " + left_->toString();
	case PredicateOp::Exists:
		return "\u2203" + name_ + " " + left_->toString();
	}
	throw std::logic_error("Nepoznat operator");
}

// ---------------------------------------------------------------------------
// Interpretation
// ---------------------------------------------------------------------------

void Interpretation::setDomain(std::vector<std::string> elements) {
	domain_ = std::move(elements);
}

void Interpretation::addFact(const std::string& predicateName, Tuple arguments) {
	facts_[predicateName].insert(std::move(arguments));
}

bool Interpretation::holds(const std::string& predicateName, const Tuple& arguments) const {
	const auto it = facts_.find(predicateName);
	return it != facts_.end() && it->second.count(arguments) > 0;
}

bool Interpretation::evaluate(const PredicateExpression::Ptr& expr) const {
	Assignment assignment;
	return evaluate(expr, assignment);
}

bool Interpretation::evaluate(const PredicateExpression::Ptr& expr, Assignment& assignment) const {
	switch (expr->op()) {
	case PredicateOp::Predicate: {
		Tuple resolved;
		resolved.reserve(expr->arguments().size());
		for (const auto& arg : expr->arguments()) {
			const auto it = assignment.find(arg);
			if (it != assignment.end()) {
				resolved.push_back(it->second);
			} else {
				// Slobodni simbol mora biti element domene (konstanta).
				bool inDomain = false;
				for (const auto& element : domain_) {
					if (element == arg) { inDomain = true; break; }
				}
				if (!inDomain) {
					throw std::invalid_argument("Slobodna varijabla '" + arg +
												"' nije vezana ni element domene");
				}
				resolved.push_back(arg);
			}
		}
		return holds(expr->name(), resolved);
	}
	case PredicateOp::Not:
		return !evaluate(expr->left(), assignment);
	case PredicateOp::And:
		return evaluate(expr->left(), assignment) && evaluate(expr->right(), assignment);
	case PredicateOp::Or:
		return evaluate(expr->left(), assignment) || evaluate(expr->right(), assignment);
	case PredicateOp::Implies:
		return !evaluate(expr->left(), assignment) || evaluate(expr->right(), assignment);
	case PredicateOp::Iff:
		return evaluate(expr->left(), assignment) == evaluate(expr->right(), assignment);
	case PredicateOp::ForAll:
	case PredicateOp::Exists: {
		const bool isForAll = expr->op() == PredicateOp::ForAll;
		const auto& variable = expr->name();
		const auto previous = assignment.find(variable) != assignment.end()
								  ? std::optional<std::string>(assignment[variable])
								  : std::optional<std::string>();
		bool result = isForAll;
		for (const auto& element : domain_) {
			assignment[variable] = element;
			const bool value = evaluate(expr->left(), assignment);
			if (isForAll && !value) { result = false; break; }
			if (!isForAll && value) { result = true; break; }
		}
		if (previous) {
			assignment[variable] = *previous;
		} else {
			assignment.erase(variable);
		}
		return result;
	}
	}
	throw std::logic_error("Nepoznat operator");
}

// ---------------------------------------------------------------------------
// PredicateParser
// ---------------------------------------------------------------------------

namespace {

class Parser {
public:
	explicit Parser(const std::string& text) : text_(text) {}

	PredicateExpression::Ptr run() {
		auto expr = parseIff();
		skipWhitespace();
		if (pos_ != text_.size()) {
			fail("Neocekivani znak");
		}
		return expr;
	}

private:
	// ↔ (najnizi precedens, lijevo-asocijativna)
	PredicateExpression::Ptr parseIff() {
		auto lhs = parseImplies();
		while (match("\u2194") || match("<->")) {
			lhs = PredicateExpression::equivalence(lhs, parseImplies());
		}
		return lhs;
	}

	// → (desno-asocijativna)
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
		if (match("\u00AC") || match("!")) {
			return PredicateExpression::negation(parseUnary());
		}
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
		skipWhitespace();
		if (match("(")) {
			auto expr = parseIff();
			if (!match(")")) {
				fail("Ocekivana ')'");
			}
			return expr;
		}
		const auto name = parseIdentifier("Ocekivan predikat ili '('");
		if (!match("(")) {
			fail("Ocekivana '(' iza imena predikata '" + name + "'");
		}
		std::vector<std::string> arguments;
		arguments.push_back(parseIdentifier("Ocekivan argument predikata"));
		while (match(",")) {
			arguments.push_back(parseIdentifier("Ocekivan argument predikata"));
		}
		if (!match(")")) {
			fail("Ocekivana ')' iza argumenata predikata");
		}
		return PredicateExpression::predicate(name, std::move(arguments));
	}

	std::string parseIdentifier(const std::string& errorMessage) {
		skipWhitespace();
		std::size_t start = pos_;
		while (pos_ < text_.size() &&
			   (std::isalnum(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '_')) {
			++pos_;
		}
		if (start == pos_) {
			fail(errorMessage);
		}
		return text_.substr(start, pos_ - start);
	}

	// Kljucna rijec smije biti pracena samo razmakom ili '(' da se ne pomijesa s identifikatorom.
	bool matchKeyword(const std::string& keyword) {
		skipWhitespace();
		if (text_.compare(pos_, keyword.size(), keyword) != 0) {
			return false;
		}
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
		if (text_.compare(pos_, token.size(), token) == 0) {
			pos_ += token.size();
			return true;
		}
		return false;
	}

	void skipWhitespace() {
		while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
			++pos_;
		}
	}

	[[noreturn]] void fail(const std::string& message) const {
		throw std::invalid_argument(message + " na poziciji " + std::to_string(pos_));
	}

	const std::string& text_;
	std::size_t pos_ = 0;
};

void collectFreeInto(const PredicateExpression::Ptr& expr,
					 std::set<std::string>& bound, std::set<std::string>& free) {
	switch (expr->op()) {
	case PredicateOp::Predicate:
		for (const auto& arg : expr->arguments()) {
			if (bound.count(arg) == 0) {
				free.insert(arg);
			}
		}
		return;
	case PredicateOp::Not:
		collectFreeInto(expr->left(), bound, free);
		return;
	case PredicateOp::ForAll:
	case PredicateOp::Exists: {
		const bool wasBound = bound.count(expr->name()) > 0;
		bound.insert(expr->name());
		collectFreeInto(expr->left(), bound, free);
		if (!wasBound) {
			bound.erase(expr->name());
		}
		return;
	}
	default:
		collectFreeInto(expr->left(), bound, free);
		collectFreeInto(expr->right(), bound, free);
		return;
	}
}

void collectPredicatesInto(const PredicateExpression::Ptr& expr,
						   std::map<std::string, std::size_t>& predicates) {
	switch (expr->op()) {
	case PredicateOp::Predicate:
		predicates[expr->name()] = expr->arguments().size();
		return;
	case PredicateOp::Not:
	case PredicateOp::ForAll:
	case PredicateOp::Exists:
		collectPredicatesInto(expr->left(), predicates);
		return;
	default:
		collectPredicatesInto(expr->left(), predicates);
		collectPredicatesInto(expr->right(), predicates);
		return;
	}
}

} // namespace

PredicateExpression::Ptr PredicateParser::parse(const std::string& formula) {
	return Parser(formula).run();
}

std::set<std::string> PredicateParser::collectFreeVariables(const PredicateExpression::Ptr& expr) {
	std::set<std::string> bound;
	std::set<std::string> free;
	collectFreeInto(expr, bound, free);
	return free;
}

std::map<std::string, std::size_t> PredicateParser::collectPredicates(
	const PredicateExpression::Ptr& expr) {
	std::map<std::string, std::size_t> predicates;
	collectPredicatesInto(expr, predicates);
	return predicates;
}

} // namespace aksiomat
