#include "aksiomat/Interpretation.hpp"

#include "aksiomat/PredicateParser.hpp"

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>

namespace aksiomat {

void Interpretation::setDomain(std::vector<std::string> elements) {
	if (elements.empty()) {
		throw std::invalid_argument("Domena ne smije biti prazna");
	}
	std::set<std::string> unique;
	for (const auto& element : elements) {
		if (element.empty()) {
			throw std::invalid_argument("Element domene ne smije biti prazan");
		}
		if (!unique.insert(element).second) {
			throw std::invalid_argument("Element domene '" + element + "' je ponovljen");
		}
	}
	domain_ = std::move(elements);
	facts_.clear();
	factArities_.clear();
}

void Interpretation::addFact(const std::string& predicateName, Tuple arguments) {
	if (domain_.empty()) {
		throw std::logic_error("Domena mora biti postavljena prije cinjenica");
	}
	if (predicateName.empty()) {
		throw std::invalid_argument("Ime predikata ne smije biti prazno");
	}
	if (arguments.empty()) {
		throw std::invalid_argument("Cinjenica mora imati barem jedan argument");
	}
	for (const auto& argument : arguments) {
		if (std::find(domain_.begin(), domain_.end(), argument) == domain_.end()) {
			throw std::invalid_argument("Argument cinjenice '" + argument +
										"' nije element domene");
		}
	}
	const auto [arity, inserted] = factArities_.emplace(predicateName, arguments.size());
	if (!inserted && arity->second != arguments.size()) {
		throw std::invalid_argument("Cinjenice predikata '" + predicateName +
									"' koriste razlicite arnosti");
	}
	facts_[predicateName].insert(std::move(arguments));
}

bool Interpretation::holds(const std::string& predicateName, const Tuple& arguments) const {
	const auto it = facts_.find(predicateName);
	return it != facts_.end() && it->second.count(arguments) > 0;
}

bool Interpretation::evaluate(const PredicateExpression::Ptr& expr) const {
	if (domain_.empty()) {
		throw std::logic_error("Interpretacija nema postavljenu domenu");
	}
	const auto signatures = PredicateParser::collectPredicates(expr);
	for (const auto& [name, tuples] : facts_) {
		const auto signature = signatures.find(name);
		if (signature == signatures.end()) continue;
		for (const auto& tuple : tuples) {
			if (tuple.size() != signature->second) {
				throw std::invalid_argument("Cinjenica za predikat '" + name +
											"' nema ocekivanu arnost " +
											std::to_string(signature->second));
			}
		}
	}
	Assignment assignment;
	return evaluate(expr, assignment);
}

bool Interpretation::evaluate(const PredicateExpression::Ptr& expr, Assignment& assignment) const {
	auto resolveTerm = [&](const std::string& term) -> std::string {
		const auto assigned = assignment.find(term);
		if (assigned != assignment.end()) return assigned->second;
		if (std::find(domain_.begin(), domain_.end(), term) != domain_.end()) return term;
		throw std::invalid_argument("Slobodna varijabla '" + term +
									"' nije vezana ni element domene");
	};

	switch (expr->op()) {
	case PredicateOp::Predicate: {
		Tuple resolved;
		resolved.reserve(expr->arguments().size());
		for (const auto& argument : expr->arguments()) {
			const auto assigned = assignment.find(argument);
			if (assigned != assignment.end()) {
				resolved.push_back(assigned->second);
			} else if (std::find(domain_.begin(), domain_.end(), argument) != domain_.end()) {
				resolved.push_back(argument);
			} else {
				throw std::invalid_argument("Slobodna varijabla '" + argument +
											"' nije vezana ni element domene");
			}
		}
		return holds(expr->name(), resolved);
	}
	case PredicateOp::Equality:
		return resolveTerm(expr->arguments()[0]) == resolveTerm(expr->arguments()[1]);
	case PredicateOp::NotEqual:
		return resolveTerm(expr->arguments()[0]) != resolveTerm(expr->arguments()[1]);
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
		const auto existing = assignment.find(variable);
		const std::optional<std::string> previous = existing == assignment.end()
														? std::nullopt
														: std::optional(existing->second);
		bool result = isForAll;
		for (const auto& element : domain_) {
			assignment[variable] = element;
			const bool value = evaluate(expr->left(), assignment);
			if (isForAll && !value) {
				result = false;
				break;
			}
			if (!isForAll && value) {
				result = true;
				break;
			}
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

} // namespace aksiomat
