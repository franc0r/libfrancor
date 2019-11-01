/**
 * Unit test for the icp class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. October 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/icp.h"
#include "francor_algorithm/flann_point_pair_estimator.h"
#include "francor_algorithm/estimate_transform.h"

#include <chrono>

using francor::algorithm::Icp;
using francor::algorithm::FlannPointPairEstimator;
using francor::algorithm::estimateTransform;
using francor::base::Point2dVector;
using francor::base::Transform2d;
using francor::base::Angle;

TEST(Icp, Initialize)
{
  constexpr std::size_t max_iterations = 10;
  constexpr double max_rms = 100.0;
  constexpr double termination_rms = 3.0;

  Icp icp(std::make_unique<FlannPointPairEstimator>(), estimateTransform);

  icp.setMaxIterations(max_iterations);
  icp.setMaxRms(max_rms);
  icp.setTerminationRms(termination_rms);

  EXPECT_EQ(icp.getMaxIterations(), max_iterations);
  EXPECT_EQ(icp.getMaxRms(), max_rms);
  EXPECT_EQ(icp.getTerminationRms(), termination_rms);
}

TEST(Icp, EstimateTransformFullAssignment)
{
  const Point2dVector origin = { { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 }, { 5.0, 5.0 }, { 6.0, 6.0 }, { 7.0, 7.0 },
                                 { 1.5, 1.5 }, { 2.5, 2.5 }, { 3.5, 3.5 }, { 4.5, 4.5 }, { 5.5, 5.5 }, { 6.5, 6.5 }, { 7.5, 7.5 } };
  Point2dVector target(origin.size());                                 
  const Transform2d transform( { Angle::createFromDegree(30.0) }, { 0.5, 0.3 } );

  // transform origin points
  ASSERT_EQ(origin.size(), target.size());

  for (std::size_t i = 0; i < target.size(); ++i)
    target[i] = transform * origin[i];

  // estimate transform using icp
  constexpr std::size_t max_iterations = 100;
  constexpr double max_rms = 10.0;
  constexpr double termination_rms = 0.05;

  Icp icp(std::make_unique<FlannPointPairEstimator>(), estimateTransform);
  Transform2d result;

  icp.setMaxIterations(max_iterations);
  icp.setMaxRms(max_rms);
  icp.setTerminationRms(termination_rms);

  ASSERT_TRUE(icp.estimateTransform(origin, target, result));
  EXPECT_NEAR(result.rotation().phi(), Angle::createFromDegree(30), Angle::createFromDegree(0.1));
  EXPECT_NEAR(result.translation().x(), 0.5, 0.01);
  EXPECT_NEAR(result.translation().y(), 0.3, 0.01);
}

TEST(Icp, EstimateTransformFullAssignmentRectangle)
{
  Point2dVector origin;
  constexpr double edge_length = 2.0;
  constexpr double edge_length_2 = edge_length / 2.0;
  constexpr std::size_t steps = 100;
  constexpr double step_width = 2.0 / static_cast<double>(steps);
  
  // top line
  for (std::size_t i = 0; i < steps; ++i)
    origin.push_back({ static_cast<double>(i) * step_width - edge_length_2,  edge_length_2 });
  
  // right line
  for (std::size_t i = 0; i < steps; ++i)
    origin.push_back({ edge_length_2, edge_length_2 - static_cast<double>(i) * step_width });

  // bottom line
  for (std::size_t i = 0; i < steps; ++i)
    origin.push_back({ edge_length_2 - static_cast<double>(i) * step_width, -edge_length_2 });

  // left line
  for (std::size_t i = 0; i < steps / 2; ++i)
    origin.push_back({ -edge_length_2, static_cast<double>(i) * step_width - edge_length_2 });

  // create target points
  Point2dVector target(origin.size());                                 
  const Transform2d transform( { Angle::createFromDegree(-20.0) }, { 0.3, 0.2 } );

  // transform origin points
  ASSERT_EQ(origin.size(), target.size());

  for (std::size_t i = 0; i < target.size(); ++i)
    target[i] = transform * origin[i];

  // estimate transform using icp
  constexpr std::size_t max_iterations = 100;
  constexpr double max_rms = 10.0;
  constexpr double termination_rms = 0.001;

  Icp icp(std::make_unique<FlannPointPairEstimator>(), estimateTransform);
  Transform2d result;

  icp.setMaxIterations(max_iterations);
  icp.setMaxRms(max_rms);
  icp.setTerminationRms(termination_rms);

  auto start = std::chrono::system_clock::now();
  ASSERT_TRUE(icp.estimateTransform(origin, target, result));
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "elapsed = " << elapsed.count() << " us" << std::endl;  
  EXPECT_NEAR(result.rotation().phi(), Angle::createFromDegree(-20), Angle::createFromDegree(1.0));
  EXPECT_NEAR(result.translation().x(), 0.3, 0.01);
  EXPECT_NEAR(result.translation().y(), 0.2, 0.01);
}

TEST(Icp, EstimateTransformHalfAssignment)
{
  const Point2dVector origin = { { 1.0, 1.0 }, { 2.0, 2.0 }, { 3.0, 3.0 }, { 4.0, 4.0 }, { 5.0, 5.0 }, { 6.0, 6.0 }, { 7.0, 7.0 },
                                 { 1.5, 1.5 }, { 2.5, 2.5 }, { 3.5, 3.5 }, { 4.5, 4.5 }, { 5.5, 5.5 }, { 6.5, 6.5 }, { 7.5, 7.5 } };
  Point2dVector target(origin.size() / 2);                                 
  const Transform2d transform( { Angle::createFromDegree(30.0) }, { 0.5, 0.3 } );

  // transform origin points
  ASSERT_EQ(origin.size() / 2, target.size());

  for (std::size_t i = 0; i < target.size(); ++i)
    target[i] = transform * origin[i];

  // estimate transform using icp
  constexpr std::size_t max_iterations = 100;
  constexpr double max_rms = 10.0;
  constexpr double termination_rms = 0.05;

  Icp icp(std::make_unique<FlannPointPairEstimator>(), estimateTransform);
  Transform2d result;

  icp.setMaxIterations(max_iterations);
  icp.setMaxRms(max_rms);
  icp.setTerminationRms(termination_rms);

  ASSERT_TRUE(icp.estimateTransform(origin, target, result));
  EXPECT_NEAR(result.rotation().phi(), Angle::createFromDegree(30), Angle::createFromDegree(0.1));
  EXPECT_NEAR(result.translation().x(), 0.5, 0.01);
  EXPECT_NEAR(result.translation().y(), 0.3, 0.01);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
