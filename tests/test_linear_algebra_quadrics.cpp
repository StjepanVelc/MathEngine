#include "aksiomat/linear_algebra/Quadrics.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::linear_algebra;

TEST(Quadrics, ClassifiesEllipsoid) {
	QuadricClassificationResult result = Quadrics::classifyCentral(1, 1, 1, -1);
	EXPECT_EQ(result.type, QuadricType::Ellipsoid);
}

TEST(Quadrics, ClassifiesHyperboloidOneSheet) {
	QuadricClassificationResult result = Quadrics::classifyCentral(1, 1, -1, -1);
	EXPECT_EQ(result.type, QuadricType::HyperboloidOneSheet);
}

TEST(Quadrics, ClassifiesHyperboloidTwoSheets) {
	QuadricClassificationResult result = Quadrics::classifyCentral(1, -1, -1, -1);
	EXPECT_EQ(result.type, QuadricType::HyperboloidTwoSheets);
}

TEST(Quadrics, ClassifiesEllipticCone) {
	QuadricClassificationResult result = Quadrics::classifyCentral(1, 1, -1, 0);
	EXPECT_EQ(result.type, QuadricType::EllipticCone);
}

TEST(Quadrics, ClassifiesEllipticParaboloid) {
	QuadricClassificationResult result = Quadrics::classifyParaboloid(1, 1, -1);
	EXPECT_EQ(result.type, QuadricType::EllipticParaboloid);
}

TEST(Quadrics, ClassifiesHyperbolicParaboloid) {
	QuadricClassificationResult result = Quadrics::classifyParaboloid(1, -1, -1);
	EXPECT_EQ(result.type, QuadricType::HyperbolicParaboloid);
}

TEST(Quadrics, RejectsInvalidParaboloidCoefficients) {
	EXPECT_THROW(Quadrics::classifyParaboloid(0, 1, -1), std::invalid_argument);
	EXPECT_THROW(Quadrics::classifyParaboloid(1, 1, 0), std::invalid_argument);
}
