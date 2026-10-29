#pragma once

#include <string>
#include <vector>

namespace aksiomat::integral_calculus {

// Metode numerickog integriranja po pravokutnicima/trapezima/parabolama.
enum class RiemannMethod { Left, Right, Midpoint, Trapezoid, Simpson };

struct RiemannRect {
	double x0 = 0.0;      // lijevi rub podintervala
	double x1 = 0.0;      // desni rub podintervala
	double height = 0.0;  // visina pravokutnika (kod trapeza/Simpsona: prosjecna visina)
	double area = 0.0;    // doprinos ukupnoj sumi
};

struct RiemannResult {
	std::string expression;
	std::string method;   // "left", "right", "midpoint", "trapezoid", "simpson"
	double a = 0.0;
	double b = 0.0;
	int n = 0;
	double approximation = 0.0;
	double stepWidth = 0.0;
	std::vector<RiemannRect> rectangles;
	// Gornja (Darbouxova) i donja suma na istoj podjeli, za objasnjenje definicije integrala.
	double lowerSum = 0.0;
	double upperSum = 0.0;
	// Usporedba s finom Simpsonovom aproksimacijom (n = 2000), za procjenu pogreske.
	double reference = 0.0;
	double error = 0.0;
	std::vector<std::string> steps;
};

class RiemannSums {
public:
	// Aproksimacija odredenog integrala od a do b za izraz f(x) uz n podintervala (1..5000).
	// Simpson se primjenjuje na svaki podinterval (parabola kroz rubove i sredinu), pa n moze biti bilo koji.
	// Baca std::invalid_argument za neispravan unos ili ako f nije definirana (konacna) u nekoj ispitnoj tocki.
	static RiemannResult compute(const std::string& expression, double a, double b, int n,
		RiemannMethod method, const std::string& variable = "x");

	static RiemannMethod parseMethod(const std::string& name);
	static std::string methodId(RiemannMethod method);
};

} // namespace aksiomat::integral_calculus
