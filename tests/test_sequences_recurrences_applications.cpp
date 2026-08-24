#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/sequences/Applications.hpp"
#include "aksiomat/sequences/Recurrences.hpp"

using namespace aksiomat::sequences;

TEST(SequenceRecurrences, GeneratesFibonacciAndComparesDefinitions) {
	EXPECT_EQ(Recurrences::fibonacci(1.0, 1.0, 7), (std::vector<double>{1, 1, 2, 3, 5, 8, 13}));
	EXPECT_EQ(Recurrences::fibonacci(2.0, 3.0, 1), (std::vector<double>{2}));
	EXPECT_TRUE(Recurrences::compareArithmetic(4.0, 3.0, 20).equivalent);
	EXPECT_TRUE(Recurrences::compareGeometric(4.0, -0.5, 20).equivalent);
	EXPECT_THROW(Recurrences::arithmetic(1.0, 1.0, 0), std::invalid_argument);
}

TEST(SequenceApplications, CalculatesInterestAndRepeatedGrowth) {
	const auto simple = Applications::simpleInterest(1000.0, 5.0, 3);
	EXPECT_DOUBLE_EQ(simple.finalValue, 1150.0);
	EXPECT_EQ(simple.values.size(), 4u);

	const auto compound = Applications::compoundInterest(1000.0, 10.0, 2, 1);
	EXPECT_NEAR(compound.finalValue, 1210.0, 1e-10);
	const auto population = Applications::populationGrowth(1000.0, 2.0, 3);
	EXPECT_NEAR(population.finalValue, 1061.208, 1e-9);
	const auto decrease = Applications::repeatedPercentage(200.0, -10.0, 2);
	EXPECT_DOUBLE_EQ(decrease.finalValue, 162.0);
}

TEST(SequenceApplications, BuildsAmortizationSchedule) {
	const auto zeroRate = Applications::amortization(1200.0, 0.0, 1, 12);
	EXPECT_DOUBLE_EQ(zeroRate.periodicPayment, 100.0);
	EXPECT_EQ(zeroRate.schedule.size(), 12u);
	EXPECT_NEAR(zeroRate.totalInterest, 0.0, 1e-10);
	EXPECT_DOUBLE_EQ(zeroRate.schedule.back().remainingBalance, 0.0);

	const auto loan = Applications::amortization(10000.0, 6.0, 1, 12);
	EXPECT_GT(loan.periodicPayment, 800.0);
	EXPECT_GT(loan.totalInterest, 0.0);
	EXPECT_NEAR(loan.schedule.back().remainingBalance, 0.0, 1e-12);
}

TEST(SequenceApplications, RejectsInvalidModels) {
	EXPECT_THROW(Applications::simpleInterest(-1.0, 5.0, 2), std::invalid_argument);
	EXPECT_THROW(Applications::compoundInterest(100.0, 5.0, 0), std::invalid_argument);
	EXPECT_THROW(Applications::repeatedPercentage(100.0, -101.0, 2), std::invalid_argument);
	EXPECT_THROW(Applications::amortization(1000.0, -1.0, 1), std::invalid_argument);
}
