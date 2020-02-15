/**
 * Unit test for the line class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_base/line.h"

using francor::base::Angle;
using francor::base::Line;
using francor::base::Point2d;

// check if the line is successfully constructed
TEST(LineTest, ConstructFromParameter)
{
  constexpr double phi = M_PI_4;
  const Line line(phi, { 0.0, 1.0 });

  EXPECT_NEAR(line.phi(), phi, 1e-6);
  EXPECT_NEAR(line.x0(), -1.0, 1e-6);
  EXPECT_NEAR(line.y0(), 1.0 , 1e-6);
}

// check if the line is successfully constructed
TEST(LineTest, ConstructFromVectorPoint)
{
  // construct a line with m = 3.0 and t = 1.0
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 3.0).normalized(), { 1.0, 3.0 }));

  EXPECT_NEAR(line.phi(), std::atan2(3.0, 1.0), 1e-6);
  EXPECT_NEAR(line.x0(), 0.0, 1e-6);
  EXPECT_NEAR(line.y0(), 0.0, 1e-6);
}

// check if the line is successfully constructed in case of an vertical line
TEST(LineTest, ConstructFromVerticalVectorPoint)
{
  // construct a line with m = inf and t = -inf
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(0.0, 1.0).normalized(), { 100.0, 1.0 })); 

  EXPECT_NEAR(line.phi(), M_PI_2,  1e-6);
  EXPECT_NEAR(line.x0(), 100.0, 1e-6);
  EXPECT_NE(line.y0(), -1e10);
}

// check if m is calculated well from a negative pointing direction vector
TEST(LineTest, ConstructFromNegativeDirectionVector)
{
  // construct a line with m = inf and t = -inf
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(-1.0, -1.0).normalized(), { 1.0, 1.0 }));

  EXPECT_NEAR(line.phi(), std::atan(1.0), 1e-6);
  EXPECT_NEAR(line.x0(), 0.0, 1e-6);
  EXPECT_NEAR(line.y0(), 0.0, 1e-6);
}

TEST(LineTest, Normal)
{
  // construct a line with m = 3.0 and t = 1.0
  const auto v = Eigen::Vector2d(1.0, 3.0).normalized();
  const Line line(Line::createFromVectorAndPoint(v, { 0.0, 1.0 }));

  EXPECT_NEAR(line.n().x(),  v.y(), 1e-6);
  EXPECT_NEAR(line.n().y(), -v.x(), 1e-6);
}

TEST(LineTest, IntersectionPoint)
{
  // construct a line with m = 3.0 and t = 1.0 and a second with m = 0 and t = 4.
  // the lines should intersect at x = 1.0 and y = 4.0
  const Line lineA(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 3.0).normalized(), { 0.0, 1.0 }));
  const Line lineB(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 0.0).normalized(), { 0.0, 4.0 }));

  const auto intersection = lineA.intersectionPoint(lineB);

  EXPECT_NEAR(intersection.x(), 1.0, 1e-6);
  EXPECT_NEAR(intersection.y(), 4.0, 1e-6);
}

TEST(LineTest, IntersectionPointBehindP0)
{
  // construct a line with m = 3.0 and t = 1.0 and a second with m = 0 and t = 4.
  // the lines should intersect at x = 1.0 and y = 4.0
  const Line lineA(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 3.0).normalized(), { 0.0, 3.0 }));
  const Line lineB(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 0.0).normalized(), { 1.0, 0.0 }));

  const auto intersection = lineA.intersectionPoint(lineB);

  EXPECT_NEAR(intersection.x(), -1.0, 1e-6);
  EXPECT_NEAR(intersection.y(), -0.0, 1e-6);
}

TEST(LineTest, DistanceTo)
{
  // construct a line with m = 3.0 and t = 1.0
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 3.0).normalized(), { 0.0, 1.0 }));
  constexpr Point2d point(-2.0, 5.0);
  const double distance = Eigen::Vector2d(1.0, 3.0).norm();

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

TEST(LineTest, DistanceToBehindP0)
{
  // construct a line with m = 3.0 and t = 1.0
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 0.0).normalized(), { 2.0, 0.0 }));
  constexpr Point2d point(1.0, 2.0);
  const double distance = point.y() - line.y0();

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}


TEST(LineTest, DistanceToHorizontalLine)
{
  // construct a line with m = 0.0 and t = 1.0
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(1.0, 0.0).normalized(), { 0.0, 1.0 }));
  constexpr Point2d point(0.5, 2.0);
  const double distance = 1.0;

  EXPECT_NEAR(line.phi(), 0.0, 1e-6);
  EXPECT_NEAR(line.y0() , 1.0, 1e-6);

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

TEST(LineTest, DistanceToVerticalLine)
{
  // construct a line with m = 3.0 and t = 1.0
  const Line line(Line::createFromVectorAndPoint(Eigen::Vector2d(0.0, 3.0).normalized(), { 0.0, 1.0 }));
  constexpr Point2d point(-2.0, 5.0);
  constexpr double distance = 2.0;

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}