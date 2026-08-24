#include "aksiomat/sequences/Applications.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::sequences {
namespace {

constexpr std::size_t maximumPeriods = 10000;

void requireFinite(double value, const char* message) {
	if (!std::isfinite(value)) {
		throw std::invalid_argument(message);
	}
}

void requireNonNegative(double value, const char* message) {
	requireFinite(value, message);
	if (value < 0.0) {
		throw std::invalid_argument(message);
	}
}

void requirePeriods(std::size_t periods, const char* message) {
	if (periods == 0 || periods > maximumPeriods) {
		throw std::invalid_argument(message);
	}
}

GrowthResult buildGrowth(double initialValue, std::vector<double> values) {
	const double finalValue = values.back();
	return {initialValue, finalValue, finalValue - initialValue, std::move(values)};
}

GrowthResult periodicGrowth(double initialValue, double ratePercent, std::size_t periods) {
	requireNonNegative(initialValue, "Initial value must be finite and non-negative");
	requireFinite(ratePercent, "Rate must be finite");
	requirePeriods(periods, "Periods must be between 1 and 10000");
	const double factor = 1.0 + ratePercent / 100.0;
	if (factor < 0.0) {
		throw std::invalid_argument("Percentage decrease cannot exceed 100 percent");
	}

	std::vector<double> values;
	values.reserve(periods + 1);
	values.push_back(initialValue);
	for (std::size_t period = 1; period <= periods; ++period) {
		const double next = values.back() * factor;
		requireFinite(next, "Repeated growth overflowed");
		values.push_back(next);
	}
	return buildGrowth(initialValue, std::move(values));
}

} // namespace

GrowthResult Applications::simpleInterest(double principal, double annualRatePercent, std::size_t years) {
	requireNonNegative(principal, "Principal must be finite and non-negative");
	requireFinite(annualRatePercent, "Annual rate must be finite");
	requirePeriods(years, "Years must be between 1 and 10000");
	std::vector<double> values;
	values.reserve(years + 1);
	for (std::size_t year = 0; year <= years; ++year) {
		const double value = principal * (1.0 + annualRatePercent / 100.0 * static_cast<double>(year));
		requireFinite(value, "Simple interest overflowed");
		if (value < 0.0) {
			throw std::invalid_argument("Simple interest produced a negative balance");
		}
		values.push_back(value);
	}
	return buildGrowth(principal, std::move(values));
}

GrowthResult Applications::compoundInterest(double principal, double annualRatePercent, std::size_t years,
	std::size_t compoundsPerYear) {
	requireNonNegative(principal, "Principal must be finite and non-negative");
	requireFinite(annualRatePercent, "Annual rate must be finite");
	requirePeriods(years, "Years must be between 1 and 10000");
	requirePeriods(compoundsPerYear, "Compounds per year must be between 1 and 10000");
	if (years > maximumPeriods / compoundsPerYear) {
		throw std::invalid_argument("Total compounding periods cannot exceed 10000");
	}
	const std::size_t periods = years * compoundsPerYear;
	const double periodicRate = annualRatePercent / 100.0 / static_cast<double>(compoundsPerYear);
	if (1.0 + periodicRate < 0.0) {
		throw std::invalid_argument("Periodic decrease cannot exceed 100 percent");
	}
	std::vector<double> values;
	values.reserve(periods + 1);
	values.push_back(principal);
	for (std::size_t period = 1; period <= periods; ++period) {
		const double next = values.back() * (1.0 + periodicRate);
		requireFinite(next, "Compound interest overflowed");
		values.push_back(next);
	}
	return buildGrowth(principal, std::move(values));
}

GrowthResult Applications::populationGrowth(double initialPopulation, double ratePercent, std::size_t periods) {
	return periodicGrowth(initialPopulation, ratePercent, periods);
}

GrowthResult Applications::repeatedPercentage(double initialValue, double changePercent, std::size_t periods) {
	return periodicGrowth(initialValue, changePercent, periods);
}

AmortizationResult Applications::amortization(double principal, double annualRatePercent,
	std::size_t years, std::size_t paymentsPerYear) {
	requireNonNegative(principal, "Principal must be finite and non-negative");
	if (principal == 0.0) {
		throw std::invalid_argument("Principal must be greater than zero");
	}
	requireFinite(annualRatePercent, "Annual rate must be finite");
	if (annualRatePercent < 0.0) {
		throw std::invalid_argument("Annual amortization rate cannot be negative");
	}
	requirePeriods(years, "Years must be between 1 and 10000");
	requirePeriods(paymentsPerYear, "Payments per year must be between 1 and 10000");
	if (years > maximumPeriods / paymentsPerYear) {
		throw std::invalid_argument("Total payments cannot exceed 10000");
	}

	const std::size_t paymentCount = years * paymentsPerYear;
	const double periodicRate = annualRatePercent / 100.0 / static_cast<double>(paymentsPerYear);
	const double payment = periodicRate == 0.0
		? principal / static_cast<double>(paymentCount)
		: principal * periodicRate / (1.0 - std::pow(1.0 + periodicRate, -static_cast<double>(paymentCount)));
	requireFinite(payment, "Amortization payment overflowed");

	std::vector<AmortizationPayment> schedule;
	schedule.reserve(paymentCount);
	double balance = principal;
	double totalPaid = 0.0;
	for (std::size_t period = 1; period <= paymentCount; ++period) {
		const double interest = balance * periodicRate;
		const double actualPayment = period == paymentCount ? balance + interest : payment;
		const double principalPart = actualPayment - interest;
		balance = std::max(0.0, balance - principalPart);
		totalPaid += actualPayment;
		schedule.push_back({period, actualPayment, interest, principalPart, balance});
	}
	return {payment, totalPaid, totalPaid - principal, std::move(schedule)};
}

} // namespace aksiomat::sequences
