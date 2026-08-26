#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "aksiomat/analytic_geometry/Circles.hpp"
#include "aksiomat/analytic_geometry/Conics.hpp"

using namespace aksiomat::analytic_geometry;

TEST(AnalyticGeometryCircles, BuildsAndAnalyzesCircles) {
	const auto general = Circles::fromGeneralEquation(-4, 6, -12);
	EXPECT_DOUBLE_EQ(general.center.x, 2); EXPECT_DOUBLE_EQ(general.center.y, -3); EXPECT_DOUBLE_EQ(general.radius, 5);
	const auto through = Circles::throughThreePoints({5, 0}, {0, 5}, {-5, 0});
	EXPECT_NEAR(through.center.x, 0, 1e-12); EXPECT_NEAR(through.center.y, 0, 1e-12); EXPECT_NEAR(through.radius, 5, 1e-12);
	const auto analysis = Circles::analyze(general, 21);
	EXPECT_NEAR(analysis.area, 25 * std::acos(-1.0), 1e-12); EXPECT_EQ(analysis.samples.size(), 21u);
}

TEST(AnalyticGeometryCircles, ClassifiesPointsAndRejectsInvalidCircles) {
	const Circle circle{{0, 0}, 5};
	EXPECT_EQ(Circles::classify({0, 0}, circle), PointCirclePosition::Inside);
	EXPECT_EQ(Circles::classify({3, 4}, circle), PointCirclePosition::On);
	EXPECT_EQ(Circles::classify({6, 0}, circle), PointCirclePosition::Outside);
	EXPECT_THROW(Circles::fromCenterRadius({0, 0}, 0), std::invalid_argument);
	EXPECT_THROW(Circles::fromGeneralEquation(0, 0, 1), std::invalid_argument);
	EXPECT_THROW(Circles::throughThreePoints({0, 0}, {1, 1}, {2, 2}), std::invalid_argument);
}

TEST(AnalyticGeometryConics, AnalyzesParabolas) {
	const auto vertical = Conics::analyze(Parabola{{1, 2}, 2, ParabolaOrientation::Vertical}, 4, 21);
	EXPECT_DOUBLE_EQ(vertical.focus.x, 1); EXPECT_DOUBLE_EQ(vertical.focus.y, 4); EXPECT_DOUBLE_EQ(vertical.directrix, 0);
	EXPECT_EQ(vertical.samples.size(), 21u);
	const auto horizontal = Conics::analyze(Parabola{{0, 0}, -1, ParabolaOrientation::Horizontal}, 3, 11);
	EXPECT_DOUBLE_EQ(horizontal.focus.x, -1); EXPECT_DOUBLE_EQ(horizontal.directrix, 1);
}

TEST(AnalyticGeometryConics, AnalyzesEllipsesAndHyperbolas) {
	const auto ellipse = Conics::analyze(Ellipse{{0, 0}, 5, 3, true}, 25);
	EXPECT_DOUBLE_EQ(ellipse.focalDistance, 4); EXPECT_DOUBLE_EQ(ellipse.eccentricity, 0.8); EXPECT_DOUBLE_EQ(ellipse.focus2.x, 4);
	const auto hyperbola = Conics::analyze(Hyperbola{{1, -1}, 3, 4, true}, 2, 31);
	EXPECT_DOUBLE_EQ(hyperbola.focalDistance, 5); EXPECT_NEAR(hyperbola.eccentricity, 5.0 / 3.0, 1e-12); EXPECT_EQ(hyperbola.branch1.size(), 31u);
}

TEST(AnalyticGeometryConics, RejectsDegenerateConics) {
	EXPECT_THROW(Conics::analyze(Parabola{{0, 0}, 0, ParabolaOrientation::Vertical}), std::invalid_argument);
	EXPECT_THROW(Conics::analyze(Ellipse{{0, 0}, 2, 3, true}), std::invalid_argument);
	EXPECT_THROW(Conics::analyze(Hyperbola{{0, 0}, -1, 2, true}), std::invalid_argument);
}
