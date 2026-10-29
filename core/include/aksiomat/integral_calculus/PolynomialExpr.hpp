#pragma once

#include <optional>
#include <string>
#include <vector>

#include "aksiomat/differential_calculus/FunctionExpression.hpp"

namespace aksiomat::integral_calculus {

// Koeficijenti polinoma: c[i] stoji uz x^i. Prazan vektor nije dopusten (nula je {0}).
using Coefficients = std::vector<double>;

// Pretvara izraz u polinom (zbroj, razlika, umnozak, potencija s prirodnim eksponentom,
// dijeljenje konstantom, konstantni izrazi). Vraca nullopt ako izraz nije polinom.
std::optional<Coefficients> toPolynomial(const differential_calculus::FnExpr::Ptr& expression);

// Stupanj polinoma (nula ima stupanj 0).
int polynomialDegree(const Coefficients& c);

// Gradi izraz oblika a_n x^n + ... + a_0 (od najviseg stupnja prema nizem).
differential_calculus::FnExpr::Ptr fromPolynomial(const Coefficients& c, const std::string& variable = "x");

} // namespace aksiomat::integral_calculus
