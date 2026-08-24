#pragma once

#include <cstddef>
#include <vector>

namespace aksiomat::sequences {

struct GrowthResult {
	double initialValue;
	double finalValue;
	double totalChange;
	std::vector<double> values;
};

struct AmortizationPayment {
	std::size_t period;
	double payment;
	double interest;
	double principal;
	double remainingBalance;
};

struct AmortizationResult {
	double periodicPayment;
	double totalPaid;
	double totalInterest;
	std::vector<AmortizationPayment> schedule;
};

class Applications {
public:
	static GrowthResult simpleInterest(double principal, double annualRatePercent, std::size_t years);
	static GrowthResult compoundInterest(double principal, double annualRatePercent, std::size_t years,
		std::size_t compoundsPerYear = 1);
	static GrowthResult populationGrowth(double initialPopulation, double ratePercent, std::size_t periods);
	static GrowthResult repeatedPercentage(double initialValue, double changePercent, std::size_t periods);
	static AmortizationResult amortization(double principal, double annualRatePercent,
		std::size_t years, std::size_t paymentsPerYear = 12);
};

} // namespace aksiomat::sequences
