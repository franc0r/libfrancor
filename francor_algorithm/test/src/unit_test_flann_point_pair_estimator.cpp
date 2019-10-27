/**
 * Unit test for the flann point pair estimator class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/flann_point_pair_estimator.h"

using francor::algorithm::FlannPointPairEstimator;
using francor::base::Point2dVector;
using francor::base::Point2d;
using francor::algorithm::PointPairIndexVector;

TEST(FlannPointPairEstimator, Instansiate)
{
  FlannPointPairEstimator estimator;
}

TEST(FlannPointPairEstimator, SetPointDataset)
{
  FlannPointPairEstimator estimator;
  const Point2dVector points = { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 } };

  EXPECT_TRUE(estimator.setPointDataset(points));
}

TEST(FlannPointPairEstimator, EstimatePairs)
{
  FlannPointPairEstimator estimator;
  const Point2dVector model_points  = { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 } };
  const Point2dVector target_points = { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 } };
  ASSERT_EQ(model_points.size(), target_points.size());

  // set point dataset
  ASSERT_TRUE(estimator.setPointDataset(model_points));
  
  // estimate point pairs
  PointPairIndexVector pairs;
  ASSERT_TRUE(estimator.findPairs(target_points, pairs));
  EXPECT_EQ(pairs.size(), model_points.size());

  // check pairs
  for (std::size_t i = 0; i < pairs.size(); ++i)
    EXPECT_EQ(model_points[pairs[i].first], target_points[pairs[i].second]);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
