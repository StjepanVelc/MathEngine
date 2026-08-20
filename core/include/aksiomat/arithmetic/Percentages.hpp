#pragma once

namespace aksiomat {

class Percentages {
public:
	static double of(double percentage, double value);
	static double increase(double value, double percentage);
	static double decrease(double value, double percentage);
	static double ratio(double part, double whole);
	static double originalBeforeIncrease(double finalValue, double percentage);
	static double originalBeforeDecrease(double finalValue, double percentage);
};

} // namespace aksiomat
