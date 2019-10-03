/**
 * Unit test for the line class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_base/line.h"

// check if the line is successfully constructed
TEST(LineTest, ConstructFromParameter)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(1.249045772, 1.0);

  EXPECT_NEAR(line.m(), 3.0, 1e-6);
  EXPECT_DOUBLE_EQ(line.t(), 1.0);

  EXPECT_NEAR(line.v().norm(), 1.0, 0.001);
  EXPECT_NEAR(line.v().x(), 1.0 / std::sqrt(1.0 * 1.0 + 3.0 * 3.0), 0.001);
  EXPECT_NEAR(line.v().y(), 3.0 / std::sqrt(1.0 * 1.0 + 3.0 * 3.0), 0.001);
  EXPECT_NEAR(line.p().x(), 0.0, 0.001);
  EXPECT_NEAR(line.p().y(), 1.0, 0.001);
}

// check if the line is successfully constructed
TEST(LineTest, ConstructFromVectorPoint)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(1.0, 3.0));

  EXPECT_NEAR(line.m(), 3.0, 0.001);
  EXPECT_NEAR(line.t(), 0.0, 0.001);

  EXPECT_NEAR(line.v().norm(), 1.0, 0.001);
  EXPECT_NEAR(line.v().x(), 1.0 / std::sqrt(1.0 * 1.0 + 3.0 * 3.0), 0.001);
  EXPECT_NEAR(line.v().y(), 3.0 / std::sqrt(1.0 * 1.0 + 3.0 * 3.0), 0.001);
  EXPECT_NEAR(line.p().x(), 1.0, 0.001);
  EXPECT_NEAR(line.p().y(), 3.0, 0.001);
}

// check if the line is successfully constructed in case of an horizontal line
// TEST(LineTest, ConstructFromVerticalVectorPoint)
// {
//   // construct a line with m = inf and t = -inf
//   const francor::base::Line line(Eigen::Vector2d(0.0, 1.0).normalized(), Eigen::Vector2d(100.0, 1.0));

//   EXPECT_NEAR(line.m(),  static_cast<double>(std::numeric_limits<std::size_t>::max()), 1e-6);
//   EXPECT_GE(line.t(), -static_cast<double>(std::numeric_limits<std::size_t>::max()));
// }

// check if m is calculated well from a negative pointing direction vector
TEST(LineTest, ConstructFromNegativeDirectionVector)
{
  // construct a line with m = inf and t = -inf
  const francor::base::Line line(Eigen::Vector2d(-1.0, -1.0).normalized(), Eigen::Vector2d(1.0, 1.0));

  EXPECT_NEAR(line.m(), 1.0, 1e-6);
  EXPECT_NEAR(line.t(), 0.0, 1e-6);
}

TEST(LineTest, Normal)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));

  EXPECT_DOUBLE_EQ(line.n().x(), Eigen::Vector2d(-3.0, 1.0).normalized().x());
  EXPECT_DOUBLE_EQ(line.n().y(), Eigen::Vector2d(-3.0, 1.0).normalized().y());
}

TEST(LineTest, NormalWithPointAbove)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const Eigen::Vector2d point(-2.0, 5.0);

  EXPECT_DOUBLE_EQ(line.n(point).x(), Eigen::Vector2d(-3.0, 1.0).normalized().x());
  EXPECT_DOUBLE_EQ(line.n(point).y(), Eigen::Vector2d(-3.0, 1.0).normalized().y());
}

TEST(LineTest, NormalWithPointBelow)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const Eigen::Vector2d point(2.0, -5.0);

  EXPECT_DOUBLE_EQ(line.n(point).x(), Eigen::Vector2d(3.0, -1.0).normalized().x());
  EXPECT_DOUBLE_EQ(line.n(point).y(), Eigen::Vector2d(3.0, -1.0).normalized().y());
}

TEST(LineTest, IntersectionPoint)
{
  // construct a line with m = 3.0 and t = 1.0 and a second with m = 0 and t = 4.
  // the lines should intersect at x = 1.0 and y = 4.0
  const francor::base::Line lineA(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const francor::base::Line lineB(Eigen::Vector2d(1.0, 0.0).normalized(), Eigen::Vector2d(0.0, 4.0));

  const auto intersection = lineA.intersectionPoint(lineB);

  EXPECT_NEAR(intersection.x(), 1.0, 1e-6);
  EXPECT_NEAR(intersection.y(), 4.0, 1e-6);
}

TEST(LineTest, DistanceTo)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const Eigen::Vector2d point(-2.0, 5.0);
  const double distance = Eigen::Vector2d(1.0, 3.0).norm();

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

TEST(LineTest, DistanceToHorizontalLine)
{
  // construct a line with m = 0.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(1.0, 0.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const Eigen::Vector2d point(0.5, 2.0);
  const double distance = 1.0;

  EXPECT_NEAR(line.m(), 0.0, 1e-3);
  EXPECT_NEAR(line.t(), 1.0, 1e-3);

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

TEST(LineTest, DistanceToVerticalLine)
{
  // construct a line with m = 3.0 and t = 1.0
  const francor::base::Line line(Eigen::Vector2d(0.0, 3.0).normalized(), Eigen::Vector2d(0.0, 1.0));
  const Eigen::Vector2d point(-2.0, 5.0);
  constexpr double distance = 2.0;

  EXPECT_NEAR(line.distanceTo(point), distance, 0.001);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}