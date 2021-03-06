/**
 * Unit test for the flann point pair estimator class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

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

TEST(FlannPointPairEstimator, EstimatePairsMixedOrder)
{
  FlannPointPairEstimator estimator;
  const Point2dVector model_points  = { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 } };
  const Point2dVector target_points = { { 4.0, 4.0 }, { 0.0, 0.0 }, { 1.0, 1.0 }, { 3.0, 3.0 }, { 2.0, 2.0 } };
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

TEST(FlannPointPairEstimator, Benchmark)
{
  FlannPointPairEstimator estimator;
  Point2dVector model_points;
  
  for (std::size_t i = 0; i < 400; ++i)
    model_points.push_back( { static_cast<double>(i), static_cast<double>(i) } );

  Point2dVector target_points;

  for (std::size_t i = 0; i < 400; ++i)
    target_points.push_back( { static_cast<double>(i), static_cast<double>(i) } );

  ASSERT_EQ(model_points.size(), target_points.size());
  auto start = std::chrono::system_clock::now();
  // set point dataset
  ASSERT_TRUE(estimator.setPointDataset(model_points));
  auto end_set = std::chrono::system_clock::now(); 
  // estimate point pairs
  PointPairIndexVector pairs;
  ASSERT_TRUE(estimator.findPairs(target_points, pairs));
  auto end = std::chrono::system_clock::now();
  EXPECT_EQ(pairs.size(), model_points.size());

  // check pairs
  for (std::size_t i = 0; i < pairs.size(); ++i)
    EXPECT_EQ(model_points[pairs[i].first], target_points[pairs[i].second]);

  auto elapsed_set = std::chrono::duration_cast<std::chrono::microseconds>(end_set - start);
  auto elapsed     = std::chrono::duration_cast<std::chrono::microseconds>(end     - start);
  std::cout << "elapsed set point dataset = " << elapsed_set.count() << " us" << std::endl;  
  std::cout << "elapsed = " << elapsed.count() << " us" << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
