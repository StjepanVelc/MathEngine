#pragma once

#include <set>
#include <string>

#include "aksiomat/LogicExpression.hpp"

namespace aksiomat {

// Parser logičkih formula.
// Podržani operatori (Unicode i ASCII alternativa):
//   negacija:      ¬  ili  !
//   konjunkcija:   ∧  ili  &
//   disjunkcija:   ∨  ili  |
//   implikacija:   →  ili  ->
//   ekvivalencija: ↔  ili  <->
// Varijable: identifikatori (npr. p, q, r1). Konstante: T/F ili 1/0.
// Precedens (od najjačeg): ¬, ∧, ∨, → (desno-asocijativna), ↔.
// Baca std::invalid_argument za neispravnu formulu.
class LogicParser {
public:
	static LogicExpression::Ptr parse(const std::string& formula);

	// Skuplja imena svih varijabli u izrazu (abecedno sortirano).
	static std::set<std::string> collectVariables(const LogicExpression::Ptr& expr);
};

} // namespace aksiomat
