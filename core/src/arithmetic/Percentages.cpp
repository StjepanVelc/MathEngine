#include "aksiomat/arithmetic/Percentages.hpp"

#include <cmath>
#include <stdexcept>

namespace aksiomat {
namespace {

void requireFinite(double value) {
	if (!std::isfinite(value)) throw std::invalid_argument("Vrijednost mora biti konacan broj");
}

double checked(double value) {
	requireFinite(value);
	return value;
}

} // namespace

double Percentages::of(double percentage, double value) {
	requireFinite(percentage);
	requireFinite(value);
	return checked(value * percentage / 100.0);
}

double Percentages::increase(double value, double percentage) {
	return checked(value + of(percentage, value));
}

double Percentages::decrease(double value, double percentage) {
	return checked(value - of(percentage, value));
}

double Percentages::ratio(double part, double whole) {
	requireFinite(part);
	requireFinite(whole);
	if (whole == 0.0) throw std::invalid_argument("Cjelina ne smije biti nula");
	return checked(part / whole * 100.0);
}

double Percentages::originalBeforeIncrease(double finalValue, double percentage) {
	requireFinite(finalValue);
	requireFinite(percentage);
	const double factor = 1.0 + percentage / 100.0;
	if (factor == 0.0) throw std::invalid_argument("Postotak povecanja daje nulti faktor");
	return checked(finalValue / factor);
}

double Percentages::originalBeforeDecrease(double finalValue, double percentage) {
	requireFinite(finalValue);
	requireFinite(percentage);
	const double factor = 1.0 - percentage / 100.0;
	if (factor == 0.0) throw std::invalid_argument("Postotak smanjenja daje nulti faktor");
	return checked(finalValue / factor);
}

} // namespace aksiomat
