#include "aksiomat/differential_calculus/FunctionStudy.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

#include "aksiomat/differential_calculus/Differentiation.hpp"
#include "aksiomat/differential_calculus/FunctionExpression.hpp"
#include "aksiomat/differential_calculus/NumericText.hpp"
#include "aksiomat/differential_calculus/RealFunctionLimits.hpp"
#include "aksiomat/differential_calculus/RootFinding.hpp"

namespace aksiomat::differential_calculus {
namespace {

using Ptr = FnExpr::Ptr;
using Function = std::function<double(double)>;

constexpr double kInf = std::numeric_limits<double>::infinity();

std::vector<FunctionPoint> sampleExpression(const Ptr& e, double xMin, double xMax, int n) {
	std::vector<FunctionPoint> points;
	points.reserve(static_cast<std::size_t>(n) + 1);
	for (int i = 0; i <= n; ++i) {
		const double x = (i == n) ? xMax : xMin + (xMax - xMin) * i / n;
		const double y = evaluate(e, x);
		points.push_back({x, y, std::isfinite(y)});
	}
	return points;
}

std::vector<IntervalBehavior> definedIntervals(const Function& f, double a, double b) {
	constexpr int grid = 800;
	const auto defined = [&](double x) { return std::isfinite(f(x)); };
	const auto refine = [&](double good, double bad) {
		for (int i = 0; i < 60; ++i) {
			const double mid = 0.5 * (good + bad);
			if (defined(mid)) good = mid; else bad = mid;
		}
		return good;
	};
	std::vector<IntervalBehavior> result;
	double start = 0.0;
	bool inside = false;
	double previous = a;
	for (int i = 0; i <= grid; ++i) {
		const double x = (i == grid) ? b : a + (b - a) * i / grid;
		const bool ok = defined(x);
		if (ok && !inside) {
			start = (i == 0) ? x : refine(x, previous);
			inside = true;
		} else if (!ok && inside) {
			result.push_back({start, refine(previous, x), "defined"});
			inside = false;
		}
		previous = x;
	}
	if (inside) result.push_back({start, b, "defined"});
	return result;
}

std::vector<double> findPoles(const Ptr& f, double a, double b) {
	constexpr int grid = 2000;
	std::vector<double> xs(grid + 1);
	std::vector<double> mags(grid + 1);
	for (int i = 0; i <= grid; ++i) {
		xs[i] = (i == grid) ? b : a + (b - a) * i / grid;
		const double v = evaluate(f, xs[i]);
		mags[i] = std::isnan(v) ? -1.0 : std::fabs(v);
	}
	std::vector<double> poles;
	for (int i = 0; i <= grid; ++i) {
		if (mags[i] < 50.0) continue;
		const double left = i > 0 ? mags[i - 1] : 0.0;
		const double right = i < grid ? mags[i + 1] : 0.0;
		if (mags[i] < left || mags[i] < right) continue;
		double pole = xs[i];
		if (std::isfinite(mags[i])) {
			double lo = xs[i > 0 ? i - 1 : 0];
			double hi = xs[i < grid ? i + 1 : grid];
			const auto magnitude = [&](double x) {
				const double v = std::fabs(evaluate(f, x));
				return std::isnan(v) ? -1.0 : v;
			};
			for (int iteration = 0; iteration < 120; ++iteration) {
				const double m1 = lo + (hi - lo) / 3.0;
				const double m2 = hi - (hi - lo) / 3.0;
				if (magnitude(m1) < magnitude(m2)) lo = m1; else hi = m2;
			}
			pole = 0.5 * (lo + hi);
		}
		const double snapped = snapToNice(pole);
		if (std::fabs(snapped - pole) < 1e-6) pole = snapped;
		const OneSidedLimit left1 = RealFunctionLimits::oneSided(f, pole, -1);
		const OneSidedLimit right1 = RealFunctionLimits::oneSided(f, pole, 1);
		const auto infinite = [](const OneSidedLimit& l) {
			return l.kind == LimitKind::PositiveInfinity || l.kind == LimitKind::NegativeInfinity;
		};
		if (!infinite(left1) && !infinite(right1)) continue;
		bool duplicate = false;
		for (const double existing : poles) {
			if (std::fabs(existing - pole) < 1e-3 * (1.0 + std::fabs(pole))) duplicate = true;
		}
		if (!duplicate) poles.push_back(pole);
	}
	std::sort(poles.begin(), poles.end());
	return poles;
}

std::vector<IntervalBehavior> behaviorIntervals(const Function& g, std::vector<double> breaks,
	const char* positive, const char* negative, const char* zero) {
	std::sort(breaks.begin(), breaks.end());
	std::vector<double> unique;
	for (const double x : breaks) {
		if (unique.empty() || x - unique.back() > 1e-9) unique.push_back(x);
	}
	std::vector<IntervalBehavior> result;
	for (std::size_t i = 0; i + 1 < unique.size(); ++i) {
		const double p = unique[i];
		const double q = unique[i + 1];
		const double v = g(0.5 * (p + q));
		if (!std::isfinite(v)) continue;
		const char* label = std::fabs(v) < 1e-12 ? zero : (v > 0.0 ? positive : negative);
		if (!result.empty() && std::fabs(result.back().to - p) < 1e-9 && result.back().behavior == label) {
			result.back().to = q;
		} else {
			result.push_back({p, q, label});
		}
	}
	return result;
}

int signAt(const Function& g, double x) {
	const double v = g(x);
	if (std::isfinite(v) && std::fabs(v) > 1e-12) return v > 0 ? 1 : -1;
	return 0;
}

std::string numberList(const std::vector<double>& values) {
	std::string text;
	for (const double v : values) {
		if (!text.empty()) text += ", ";
		text += formatValue(snapToNice(v));
	}
	return text;
}

std::string intervalText(const IntervalBehavior& interval, const char* word) {
	return "(" + formatValue(snapToNice(interval.from)) + ", " + formatValue(snapToNice(interval.to)) + "): " + word;
}

} // namespace

std::vector<FunctionPoint> FunctionStudy::sample(const std::string& expression, double xMin, double xMax,
	int sampleCount, const std::string& variable) {
	if (!std::isfinite(xMin) || !std::isfinite(xMax) || !(xMin < xMax)) throw std::invalid_argument("Interval mora zadovoljavati xMin < xMax");
	if (sampleCount < 2 || sampleCount > 20000) throw std::invalid_argument("Broj uzoraka mora biti izmedju 2 i 20000");
	return sampleExpression(simplify(parseFunction(expression, variable)), xMin, xMax, sampleCount);
}

FunctionStudyResult FunctionStudy::analyze(const std::string& expression, double xMin, double xMax,
	int sampleCount, const std::string& variable) {
	if (!std::isfinite(xMin) || !std::isfinite(xMax) || !(xMin < xMax)) throw std::invalid_argument("Interval mora zadovoljavati xMin < xMax");
	if (xMax - xMin > 1e6) throw std::invalid_argument("Interval je prevelik");
	if (sampleCount < 10 || sampleCount > 5000) throw std::invalid_argument("Broj uzoraka mora biti izmedju 10 i 5000");

	const Ptr f = simplify(parseFunction(expression, variable));
	const Ptr d1 = simplify(Differentiation::derive(f));
	const Ptr d2 = simplify(Differentiation::derive(d1));
	const Function F = [&](double x) { return evaluate(f, x); };
	const Function D1 = [&](double x) { return evaluate(d1, x); };
	const Function D2 = [&](double x) { return evaluate(d2, x); };

	FunctionStudyResult result;
	result.expression = toString(f);
	result.firstDerivative = toString(d1);
	result.secondDerivative = toString(d2);
	result.xMin = xMin;
	result.xMax = xMax;
	result.f = sampleExpression(f, xMin, xMax, sampleCount);
	result.df = sampleExpression(d1, xMin, xMax, sampleCount);
	result.ddf = sampleExpression(d2, xMin, xMax, sampleCount);

	const std::string range = "[" + formatValue(xMin) + ", " + formatValue(xMax) + "]";
	result.steps.push_back("Ispitujemo tok funkcije f(" + variable + ") = " + result.expression + " na intervalu " + range + ".");

	result.domain = definedIntervals(F, xMin, xMax);
	if (result.domain.size() == 1 && result.domain[0].from <= xMin + 1e-9 && result.domain[0].to >= xMax - 1e-9) {
		result.steps.push_back("1. Domena: funkcija je definirana na cijelom promatranom intervalu.");
	} else if (result.domain.empty()) {
		result.steps.push_back("1. Domena: funkcija nije definirana nigdje na promatranom intervalu.");
	} else {
		std::string text = "1. Domena: funkcija je definirana na ";
		for (std::size_t i = 0; i < result.domain.size(); ++i) {
			if (i) text += " i ";
			text += "[" + formatValue(snapToNice(result.domain[i].from)) + ", " + formatValue(snapToNice(result.domain[i].to)) + "]";
		}
		result.steps.push_back(text + " (izvan toga izraz nije definiran, npr. korijen negativnog broja, logaritam nepozitivnog, dijeljenje nulom).");
	}

	result.zeros = findRoots(F, xMin, xMax);
	result.steps.push_back(result.zeros.empty()
		? "2. Nultocke (f(x) = 0): nema ih na promatranom intervalu."
		: "2. Nultocke (f(x) = 0), tamo graf sijece ili dodiruje os x: x = " + numberList(result.zeros) + ".");

	result.steps.push_back("3. Prva derivacija: f'(x) = " + result.firstDerivative +
		". Predznak f' odredjuje monotonost: f' > 0 znaci da f raste, f' < 0 da pada.");

	const std::vector<double> criticalRoots = findRoots(D1, xMin, xMax);
	std::vector<double> poles = findPoles(f, xMin, xMax);
	const double span = xMax - xMin;
	std::vector<double> monotonicBreaks = {xMin, xMax};
	monotonicBreaks.insert(monotonicBreaks.end(), criticalRoots.begin(), criticalRoots.end());
	monotonicBreaks.insert(monotonicBreaks.end(), poles.begin(), poles.end());
	for (const auto& interval : result.domain) {
		monotonicBreaks.push_back(interval.from);
		monotonicBreaks.push_back(interval.to);
	}
	result.monotonicity = behaviorIntervals(D1, monotonicBreaks, "increasing", "decreasing", "constant");

	for (const double r : criticalRoots) {
		const double y = F(r);
		if (!std::isfinite(y)) continue;
		const double delta = std::min(1e-6 * (1.0 + std::fabs(r)), span * 1e-4);
		const int before = signAt(D1, r - delta);
		const int after = signAt(D1, r + delta);
		CriticalPoint point;
		point.x = snapToNice(r);
		point.y = snapToNice(y);
		point.secondDerivative = D2(r);
		point.kind = (before > 0 && after < 0) ? "maximum" : ((before < 0 && after > 0) ? "minimum" : "stationary");
		result.criticalPoints.push_back(point);
	}

	if (result.criticalPoints.empty()) {
		result.steps.push_back("4. Stacionarne tocke (f'(x) = 0): nema ih na intervalu, pa f nema lokalnih ekstrema (u unutrasnjosti).");
	} else {
		result.steps.push_back("4. Stacionarne tocke (f'(x) = 0) i ekstremi: ekstrem moze biti samo ondje gdje je f' = 0 (ili f' ne postoji).");
		for (const auto& p : result.criticalPoints) {
			const std::string where = "x = " + formatValue(p.x) + ", f(x) = " + formatValue(p.y);
			if (p.kind == "maximum") {
				result.steps.push_back("   - lokalni MAKSIMUM u " + where + " (f' prelazi iz + u -; f'' = " + formatValue(snapToNice(p.secondDerivative)) + " < 0 potvrdjuje).");
			} else if (p.kind == "minimum") {
				result.steps.push_back("   - lokalni MINIMUM u " + where + " (f' prelazi iz - u +; f'' = " + formatValue(snapToNice(p.secondDerivative)) + " > 0 potvrdjuje).");
			} else {
				result.steps.push_back("   - stacionarna tocka bez ekstrema u " + where + " (f' ne mijenja predznak, npr. x^3 u 0).");
			}
		}
	}
	for (const auto& interval : result.monotonicity) {
		const char* word = interval.behavior == "increasing" ? "f raste" : (interval.behavior == "decreasing" ? "f pada" : "f je konstantna");
		result.steps.push_back("   Monotonost " + intervalText(interval, word));
	}

	result.steps.push_back("5. Druga derivacija: f''(x) = " + result.secondDerivative +
		". f'' > 0: graf je konveksan (zakrivljen prema gore, kao zdjela); f'' < 0: konkavan (zakrivljen prema dolje).");
	const std::vector<double> inflectionRoots = findRoots(D2, xMin, xMax);
	std::vector<double> concavityBreaks = {xMin, xMax};
	concavityBreaks.insert(concavityBreaks.end(), inflectionRoots.begin(), inflectionRoots.end());
	concavityBreaks.insert(concavityBreaks.end(), poles.begin(), poles.end());
	for (const auto& interval : result.domain) {
		concavityBreaks.push_back(interval.from);
		concavityBreaks.push_back(interval.to);
	}
	result.concavity = behaviorIntervals(D2, concavityBreaks, "convex", "concave", "linear");
	for (const double r : inflectionRoots) {
		const double y = F(r);
		if (!std::isfinite(y)) continue;
		const double delta = std::min(1e-6 * (1.0 + std::fabs(r)), span * 1e-4);
		if (signAt(D2, r - delta) * signAt(D2, r + delta) < 0) {
			result.inflectionPoints.push_back({snapToNice(r), snapToNice(y)});
		}
	}
	for (const auto& interval : result.concavity) {
		const char* word = interval.behavior == "convex" ? "konveksna (prema gore)" : (interval.behavior == "concave" ? "konkavna (prema dolje)" : "pravac (f'' = 0)");
		result.steps.push_back("   Zakrivljenost " + intervalText(interval, word));
	}
	if (result.inflectionPoints.empty()) {
		result.steps.push_back("6. Tocke infleksije: nema ih (f'' ne mijenja predznak).");
	} else {
		std::string text = "6. Tocke infleksije (f'' mijenja predznak, graf prelazi iz konveksnog u konkavni ili obrnuto): ";
		for (std::size_t i = 0; i < result.inflectionPoints.size(); ++i) {
			if (i) text += "; ";
			text += "(" + formatValue(result.inflectionPoints[i].x) + ", " + formatValue(result.inflectionPoints[i].y) + ")";
		}
		result.steps.push_back(text + ".");
	}

	for (const double pole : poles) {
		AsymptoteInfo info;
		info.kind = "vertical";
		info.x = pole;
		info.equation = "x = " + formatValue(snapToNice(pole));
		result.asymptotes.push_back(info);
	}
	const Ptr x = FnExpr::variable(variable);
	for (const double direction : {-kInf, kInf}) {
		const char* side = direction > 0 ? "+inf" : "-inf";
		const OneSidedLimit horizontal = RealFunctionLimits::oneSided(f, direction, 1);
		if (horizontal.kind == LimitKind::Finite) {
			bool duplicate = false;
			for (auto& existing : result.asymptotes) {
				if (existing.kind == "horizontal" && std::fabs(existing.offset - horizontal.value) < 1e-9) {
					existing.side = "oba kraja";
					duplicate = true;
				}
			}
			if (!duplicate) {
				AsymptoteInfo info;
				info.kind = "horizontal";
				info.offset = horizontal.value;
				info.equation = "y = " + formatValue(horizontal.value);
				info.side = side;
				result.asymptotes.push_back(info);
			}
			continue;
		}
		if (horizontal.kind != LimitKind::PositiveInfinity && horizontal.kind != LimitKind::NegativeInfinity) continue;
		const OneSidedLimit k = RealFunctionLimits::oneSided(FnExpr::binary(FnOp::Divide, f, x), direction, 1);
		if (k.kind != LimitKind::Finite || std::fabs(k.value) < 1e-9) continue;
		const Ptr remainder = FnExpr::binary(FnOp::Subtract, f, FnExpr::binary(FnOp::Multiply, FnExpr::number(k.value), x));
		const OneSidedLimit n = RealFunctionLimits::oneSided(remainder, direction, 1);
		if (n.kind != LimitKind::Finite) continue;
		AsymptoteInfo info;
		info.kind = "slant";
		info.slope = k.value;
		info.offset = n.value;
		info.side = side;
		std::string line = "y = ";
		line += (k.value == 1.0 ? "x" : (k.value == -1.0 ? "-x" : formatValue(k.value) + "*x"));
		if (n.value > 0) line += " + " + formatValue(n.value); else if (n.value < 0) line += " - " + formatValue(-n.value);
		info.equation = line;
		result.asymptotes.push_back(info);
	}
	if (result.asymptotes.empty()) {
		result.steps.push_back("7. Asimptote: nisu pronadjene (nema vertikalnih na intervalu, a limesi u +-inf nisu konacni niti linearni).");
	} else {
		result.steps.push_back("7. Asimptote (pravci kojima se graf neogranicno priblizava): vertikalne su tamo gdje |f| -> inf; vodoravne iz limesa kad x -> +-inf; kose y = kx + n s k = lim f(x)/x i n = lim (f(x) - kx).");
		for (const auto& a : result.asymptotes) {
			const char* kind = a.kind == "vertical" ? "vertikalna" : (a.kind == "horizontal" ? "vodoravna" : "kosa");
			result.steps.push_back(std::string("   - ") + kind + " asimptota " + a.equation + (a.side.empty() ? "" : " (" + a.side + ")"));
		}
	}
	return result;
}

} // namespace aksiomat::differential_calculus
