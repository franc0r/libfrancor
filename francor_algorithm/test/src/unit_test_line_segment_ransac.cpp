/**
 * Unit test for the line segment ransac class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. April 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/ransac.h"

using francor::base::Vector2d;
using francor::base::VectorVector2d;
using francor::base::LineSegment;
using francor::base::LineSegmentVector;
using francor::algorithm::RansacLineSegmentModel;
using francor::algorithm::LineSegmentRansac;

TEST(RansacLineSegmentModel, EstimateModel)
{
  RansacLineSegmentModel model;

  std::array<RansacLineSegmentModel::Input::type, RansacLineSegmentModel::Input::count> inputPoints = { Vector2d(2.0, 1.0), Vector2d(2.0, -1.0) };
  ASSERT_TRUE(model.estimate(inputPoints));

  EXPECT_NEAR(model.model().line().m(), std::numeric_limits<double>::max(), 1.0);
  EXPECT_NEAR(model.model().line().t(), -std::numeric_limits<double>::max(), 1.0);
  EXPECT_EQ(model.model().p0().x(), 2.0);
  EXPECT_EQ(model.model().p0().y(),-1.0);
  EXPECT_EQ(model.model().p1().x(), 2.0);
  EXPECT_EQ(model.model().p1().y(), 1.0);
}

TEST(LineSegmentRansac, Instancitate)
{
  LineSegmentRansac ransac;
}

TEST(LineSegmentRansac, FindTwoLineSegments)
{
  LineSegmentRansac ransac;
  const VectorVector2d inputPoints = { Vector2d(-2.0,  1.0), Vector2d(-2.0, -1.0), Vector2d(-2.0,  0.0), Vector2d(-2.0,  2.0), Vector2d(-2.0, -2.0),
                                       Vector2d( 2.0, 14.0), Vector2d( 2.0, 13.0), Vector2d( 2.0, 12.0), Vector2d( 2.0, 11.0), Vector2d( 2.0, 10.0),
                                       Vector2d(33.0, 23.0), Vector2d(25.0, 33.0) }; // the last two are outliers

  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);
  ransac.setMinNumPoints(4);                                   

  LineSegmentVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 2);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
