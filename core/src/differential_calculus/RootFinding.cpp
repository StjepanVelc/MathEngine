#include "aksiomat/differential_calculus/RootFinding.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace aksiomat::differential_calculus {

std::vector<double> findRoots(const std::function<double(double)>& g, double a, double b, int gridPoints) {
	if (!(a < b)) throw std::invalid_argument("Interval mora zadovoljavati a < b");
	if (gridPoints < 10) gridPoints = 10;

	const double h = (b - a) / gridPoints;
	std::vector<double> xs(gridPoints + 1);
	std::vector<double> gs(gridPoints + 1);
	for (int i = 0; i <= gridPoints; ++i) {
		xs[i] = (i == gridPoints) ? b : a + i * h;
		gs[i] = g(xs[i]);
	}

	std::vector<double> roots;
	for (int i = 0; i <= gridPoints; ++i) {
		if (std::isfinite(gs[i]) && gs[i] == 0.0) roots.push_back(xs[i]);
	}

	for (int i = 0; i < gridPoints; ++i) {
		if (!std::isfinite(gs[i]) || !std::isfinite(gs[i + 1])) continue;
		if (!(gs[i] * gs[i + 1] < 0.0)) continue;
		double lo = xs[i];
		double hi = xs[i + 1];
		double glo = gs[i];
		for (int iteration = 0; iteration < 100; ++iteration) {
			const double mid = 0.5 * (lo + hi);
			const double gm = g(mid);
			if (!std::isfinite(gm)) break;
			if ((glo < 0.0) == (gm < 0.0)) {
				lo = mid;
				glo = gm;
			} else {
				hi = mid;
			}
		}
		const double root = 0.5 * (lo + hi);
		const double residual = std::fabs(g(root));
		if (residual <= 1e-9 * std::max(std::fabs(gs[i]), std::fabs(gs[i + 1])) + 1e-12) roots.push_back(root);
	}

	for (int i = 1; i < gridPoints; ++i) {
		if (!std::isfinite(gs[i - 1]) || !std::isfinite(gs[i]) || !std::isfinite(gs[i + 1])) continue;
		const double m = std::fabs(gs[i]);
		if (m == 0.0 || !(m < std::fabs(gs[i - 1]) && m <= std::fabs(gs[i + 1]))) continue;
		if (!((gs[i - 1] > 0) == (gs[i] > 0) && (gs[i + 1] > 0) == (gs[i] > 0))) continue;
		double lo = xs[i - 1];
		double hi = xs[i + 1];
		for (int iteration = 0; iteration < 120; ++iteration) {
			const double m1 = lo + (hi - lo) / 3.0;
			const double m2 = hi - (hi - lo) / 3.0;
			if (std::fabs(g(m1)) < std::fabs(g(m2))) hi = m2; else lo = m1;
		}
		const double root = 0.5 * (lo + hi);
		if (std::fabs(g(root)) < 1e-9) roots.push_back(root);
	}

	std::sort(roots.begin(), roots.end());
	std::vector<double> unique;
	for (const double r : roots) {
		if (unique.empty() || std::fabs(r - unique.back()) > 1e-6 * (1.0 + std::fabs(r))) unique.push_back(r);
	}
	return unique;
}

} // namespace aksiomat::differential_calculus
