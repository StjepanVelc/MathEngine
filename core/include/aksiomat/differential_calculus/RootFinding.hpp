#pragma once

#include <functional>
#include <vector>

namespace aksiomat::differential_calculus {

// Numericki pronalazi nultocke funkcije g na [a, b]: promjene predznaka (bisekcija, polovi se odbacuju),
// tocne nule u mrezi i dodirne nultocke (npr. x^2 u 0). Rezultat je sortiran bez duplikata.
std::vector<double> findRoots(const std::function<double(double)>& g, double a, double b, int gridPoints = 2000);

} // namespace aksiomat::differential_calculus
