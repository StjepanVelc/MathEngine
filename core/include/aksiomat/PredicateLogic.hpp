#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace aksiomat {

// Vrsta cvora u stablu predikatne formule.
enum class PredicateOp {
	Predicate, // npr. P(x), Manji(x, y)
	Not,
	And,
	Or,
	Implies,
	Iff,
	ForAll, // ∀x ...
	Exists  // ∃x ...
};

// Nepromjenjivi AST cvor formule predikatne logike prvog reda
// (bez funkcijskih simbola; termi su varijable ili elementi domene).
class PredicateExpression {
public:
	using Ptr = std::shared_ptr<const PredicateExpression>;

	// Factory funkcije
	static Ptr predicate(std::string name, std::vector<std::string> arguments);
	static Ptr negation(Ptr operand);
	static Ptr conjunction(Ptr lhs, Ptr rhs);
	static Ptr disjunction(Ptr lhs, Ptr rhs);
	static Ptr implication(Ptr lhs, Ptr rhs);
	static Ptr equivalence(Ptr lhs, Ptr rhs);
	static Ptr forAll(std::string variable, Ptr body);
	static Ptr exists(std::string variable, Ptr body);

	PredicateOp op() const { return op_; }
	const std::string& name() const { return name_; } // ime predikata ili vezane varijable
	const std::vector<std::string>& arguments() const { return arguments_; }
	Ptr left() const { return left_; }
	Ptr right() const { return right_; }

	// Tekstualni prikaz, npr. "∀x (P(x) → ∃y Q(x, y))".
	std::string toString() const;

private:
	PredicateExpression(PredicateOp op, std::string name,
						std::vector<std::string> arguments, Ptr left, Ptr right);

	PredicateOp op_;
	std::string name_;
	std::vector<std::string> arguments_;
	Ptr left_;
	Ptr right_;
};

// Interpretacija nad konacnom domenom: elementi domene su stringovi,
// a svaki predikat ima skup n-torki za koje je istinit.
class Interpretation {
public:
	using Tuple = std::vector<std::string>;

	void setDomain(std::vector<std::string> elements);
	const std::vector<std::string>& domain() const { return domain_; }

	// Dodaje n-torku u ekstenziju predikata (predikat je istinit za nju).
	void addFact(const std::string& predicateName, Tuple arguments);

	bool holds(const std::string& predicateName, const Tuple& arguments) const;

	// Evaluira zatvorenu formulu (sve varijable vezane kvantifikatorima).
	// Baca std::invalid_argument za slobodnu varijablu koja nije element domene.
	bool evaluate(const PredicateExpression::Ptr& expr) const;

private:
	using Assignment = std::map<std::string, std::string>;

	bool evaluate(const PredicateExpression::Ptr& expr, Assignment& assignment) const;

	std::vector<std::string> domain_;
	std::map<std::string, std::set<Tuple>> facts_;
};

// Parser formula predikatne logike.
// Sintaksa:
//   kvantifikatori: ∀x ... / ∃x ...  (ASCII: forall x, exists x)
//   predikati:      P(x), Manji(x, y)
//   veznici kao u iskaznoj logici: ¬ ∧ ∨ → ↔ (ili ! & | -> <->)
// Precedens: ¬/kvantifikatori, ∧, ∨, → (desno-asoc.), ↔.
// Baca std::invalid_argument za neispravnu formulu.
class PredicateParser {
public:
	static PredicateExpression::Ptr parse(const std::string& formula);

	// Skuplja slobodne varijable formule (abecedno sortirano).
	static std::set<std::string> collectFreeVariables(const PredicateExpression::Ptr& expr);

	// Skuplja imena predikata s njihovim mjesnosti (arnostima), npr. {"P":1, "Q":2}.
	static std::map<std::string, std::size_t> collectPredicates(const PredicateExpression::Ptr& expr);
};

} // namespace aksiomat
