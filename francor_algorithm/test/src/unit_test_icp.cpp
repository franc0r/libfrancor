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

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
