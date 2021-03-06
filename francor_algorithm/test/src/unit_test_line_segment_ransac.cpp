/**
 * Unit test for the line segment ransac class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. April 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/ransac.h"

using francor::base::Vector2d;
using francor::base::Point2dVector;
using francor::base::LineSegment;
using francor::base::LineSegmentVector;
using francor::algorithm::RansacLineSegmentModel;
using francor::algorithm::LineSegmentRansac;

TEST(RansacLineSegmentModel, EstimateModel)
{
  RansacLineSegmentModel model;

  std::array<RansacLineSegmentModel::Input::type, RansacLineSegmentModel::Input::count> inputPoints = {{ { 2.0, 1.0 }, { 2.0, -1.0 } }};
  ASSERT_TRUE(model.estimate(inputPoints));

  EXPECT_EQ(model.model().p0().x(), 2.0);
  EXPECT_EQ(model.model().p0().y(),-1.0);
  EXPECT_EQ(model.model().p1().x(), 2.0);
  EXPECT_EQ(model.model().p1().y(), 1.0);
}

TEST(LineSegmentRansac, Instancitate)
{
  LineSegmentRansac ransac;
}

TEST(LineSegmentRansac, FindHorizontalLineSegment)
{
  LineSegmentRansac ransac;
  const Point2dVector inputPoints = { { -2.0, 1.0 }, { -1.0, 1.0 }, { 0.0, 1.0 }, { 1.0, 1.0 }, { 2.0, 1.0 } };

  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(10);
  ransac.setMinNumPoints(4);                                   

  LineSegmentVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 1);

  EXPECT_EQ(result[0].p0().x(),-2.0);
  EXPECT_EQ(result[0].p0().y(), 1.0);
  EXPECT_EQ(result[0].p1().x(), 2.0);
  EXPECT_EQ(result[0].p1().y(), 1.0);
}

TEST(LineSegmentRansac, FindVerticalLineSegment)
{
  LineSegmentRansac ransac;
  const Point2dVector inputPoints = { { -2.0,  1.0 }, { -2.0, -1.0 }, { -2.0,  0.0 }, { -2.0,  2.0 }, { -2.0, -2.0 } };

  ransac.setEpsilon(0.2);
  ransac.setMaxIterations(10);
  ransac.setMinNumPoints(4);                                   

  LineSegmentVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 1);

  EXPECT_NEAR(result[0].p0().x(), -2.0, 1e-6);
  EXPECT_NEAR(result[0].p0().y(), -2.0, 1e-6);
  EXPECT_NEAR(result[0].p1().x(), -2.0, 1e-6);
  EXPECT_NEAR(result[0].p1().y(),  2.0, 1e-6);
}

TEST(LineSegmentRansac, FindDiagonalLineSegment)
{
  LineSegmentRansac ransac;
  const Point2dVector inputPoints = { { -2.0, -2.0 }, { -1.0, -1.0 }, { 0.0,  0.0 }, { 1.0, 1.0 }, { 2.0, 2.0 } };

  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(10);
  ransac.setMinNumPoints(4);                                   

  LineSegmentVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 1);

  EXPECT_NEAR(result[0].line().phi(), std::atan(1.0), 1e-6);
  // it exists inf solutions for t
  EXPECT_NEAR(result[0].p0().x(), -2.0, 1e-6);
  EXPECT_NEAR(result[0].p0().y(), -2.0, 1e-6);
  EXPECT_NEAR(result[0].p1().x(),  2.0, 1e-6);
  EXPECT_NEAR(result[0].p1().y(),  2.0, 1e-6);
}

TEST(LineSegmentRansac, FindTwoLineSegments)
{
  LineSegmentRansac ransac;
  const Point2dVector inputPoints = { { -2.0,  1.0 }, { -2.0, -1.0 }, { -2.0,  0.0 }, { -2.0,  2.0 }, { -2.0, -2.0 },
                                      { 12.0, 14.0 }, { 12.0, 13.0 }, { 12.0, 12.0 }, { 12.0, 11.0 }, { 12.0, 10.0 },
                                      { 33.0, 23.0 }, { 25.0, 33.0 } }; // the last two are outliers

  ransac.setEpsilon(0.2);
  ransac.setMaxIterations(10);
  ransac.setMinNumPoints(4);                                   

  LineSegmentVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 2);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
