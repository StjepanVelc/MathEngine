#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "aksiomat/PredicateExpression.hpp"

namespace aksiomat {

class Interpretation {
public:
	using Tuple = std::vector<std::string>;

	void setDomain(std::vector<std::string> elements);
	const std::vector<std::string>& domain() const { return domain_; }

	void addFact(const std::string& predicateName, Tuple arguments);
	bool holds(const std::string& predicateName, const Tuple& arguments) const;
	bool evaluate(const PredicateExpression::Ptr& expr) const;

private:
	using Assignment = std::map<std::string, std::string>;

	bool evaluate(const PredicateExpression::Ptr& expr, Assignment& assignment) const;

	std::vector<std::string> domain_;
	std::map<std::string, std::set<Tuple>> facts_;
	std::map<std::string, std::size_t> factArities_;
};

} // namespace aksiomat
