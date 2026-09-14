#include "aksiomat/linear_algebra/VectorSpaces.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace aksiomat::linear_algebra {

namespace {

void validateVectors(const std::vector<std::vector<double>>& vectors) {
	if (vectors.empty()) {
		throw std::invalid_argument("Potreban je barem jedan vektor.");
	}
	const std::size_t dimension = vectors.front().size();
	if (dimension == 0) {
		throw std::invalid_argument("Vektori ne smiju biti prazni.");
	}
	for (const auto& vector : vectors) {
		if (vector.size() != dimension) {
			throw std::invalid_argument("Svi vektori moraju imati istu dimenziju.");
		}
		for (double value : vector) {
			if (!std::isfinite(value)) {
				throw std::invalid_argument("Komponente vektora moraju biti konacni brojevi.");
			}
		}
	}
}

std::string formatNumber(double value) {
	std::ostringstream stream;
	stream.precision(6);
	stream << value;
	return stream.str();
}

// Gaussova eliminacija s parcijalnim pivotiranjem; vraca reducirane retke i rang.
struct EliminationResult {
	std::vector<std::vector<double>> rows;
	int rank;
};

EliminationResult gaussianEliminate(std::vector<std::vector<double>> rows) {
	const std::size_t rowCount = rows.size();
	const std::size_t colCount = rows.front().size();
	std::size_t pivotRow = 0;
	int rank = 0;
	for (std::size_t col = 0; col < colCount && pivotRow < rowCount; ++col) {
		std::size_t maxRow = pivotRow;
		for (std::size_t r = pivotRow + 1; r < rowCount; ++r) {
			if (std::abs(rows[r][col]) > std::abs(rows[maxRow][col])) {
				maxRow = r;
			}
		}
		if (std::abs(rows[maxRow][col]) < 1e-9) {
			continue;
		}
		std::swap(rows[pivotRow], rows[maxRow]);
		for (std::size_t r = 0; r < rowCount; ++r) {
			if (r == pivotRow) continue;
			const double factor = rows[r][col] / rows[pivotRow][col];
			for (std::size_t c = col; c < colCount; ++c) {
				rows[r][c] -= factor * rows[pivotRow][c];
			}
		}
		++pivotRow;
		++rank;
	}
	return EliminationResult{rows, rank};
}

} // namespace

LinearIndependenceResult VectorSpaces::checkLinearIndependence(const std::vector<std::vector<double>>& vectors) {
	validateVectors(vectors);
	const EliminationResult elimination = gaussianEliminate(vectors);
	const bool independent = elimination.rank == static_cast<int>(vectors.size());
	std::vector<std::string> steps;
	steps.push_back("Gaussovom eliminacijom svodimo vektore na eselon oblik kako bismo odredili rang.");
	steps.push_back("Rang skupa vektora iznosi " + std::to_string(elimination.rank) + " od ukupno " + std::to_string(vectors.size()) + " vektora.");
	if (independent) {
		steps.push_back("Rang je jednak broju vektora, pa su vektori linearno nezavisni.");
	} else {
		steps.push_back("Rang je manji od broja vektora, pa su vektori linearno zavisni.");
	}
	return LinearIndependenceResult{independent, elimination.rank, std::move(steps)};
}

RankResult VectorSpaces::computeRank(const std::vector<std::vector<double>>& vectors) {
	validateVectors(vectors);
	const EliminationResult elimination = gaussianEliminate(vectors);
	std::vector<std::string> steps;
	steps.push_back("Gaussova eliminacija svodi matricu vektora na eselon oblik.");
	steps.push_back("Broj nenul pivota (rang) iznosi " + std::to_string(elimination.rank) + ".");
	return RankResult{elimination.rank, std::move(steps)};
}

BasisResult VectorSpaces::extractBasis(const std::vector<std::vector<double>>& vectors) {
	validateVectors(vectors);
	std::vector<std::vector<double>> accumulated;
	std::vector<std::vector<double>> basis;
	std::vector<std::string> steps;
	steps.push_back("Vektore dodajemo jedan po jedan i provjeravamo povecava li se rang; ako da, vektor ulazi u bazu.");
	int previousRank = 0;
	for (const auto& vector : vectors) {
		accumulated.push_back(vector);
		const EliminationResult elimination = gaussianEliminate(accumulated);
		if (elimination.rank > previousRank) {
			basis.push_back(vector);
			previousRank = elimination.rank;
		} else {
			accumulated.pop_back();
		}
	}
	steps.push_back("Izdvojena baza sadrzi " + std::to_string(basis.size()) + " vektora, sto je dimenzija razapetog potprostora.");
	return BasisResult{basis, static_cast<int>(basis.size()), std::move(steps)};
}

} // namespace aksiomat::linear_algebra
