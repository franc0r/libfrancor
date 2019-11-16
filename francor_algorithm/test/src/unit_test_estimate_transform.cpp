/**
 * Unit test for the transformation estimation functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/estimate_transform.h"

using francor::algorithm::estimateTransform;
using francor::base::Vector2d;
using francor::base::Point2d;
using francor::base::Point2dVector;
using francor::base::Transform2d;
using francor::base::Angle;
using francor::algorithm::PointPairIndexVector;

TEST(EstimateTransform, EstimateFromTwoPointSets)
{
  const Transform2d transform( { Angle::createFromDegree(10.0) }, { 1.0, 2.0 } );
  const Point2dVector origin = { { -2.0, 3.0 }, { -1.0, 3.0 }, { 0.0, 3.0 }, { 1.0, 3.0 }, { 2.0, 3.0 } };
  PointPairIndexVector pairs;
  Point2dVector transformed(origin.size());

  for (std::size_t i = 0; i < origin.size(); ++i) {
      transformed[i] = transform * origin[i];
      pairs.push_back( { i, i } );
  }

  Transform2d result;
  const auto rms = estimateTransform(origin, transformed, pairs, std::numeric_limits<double>::max(), result);

  EXPECT_GE(rms, 0.0);
  EXPECT_NEAR(result.rotation().phi() , transform.rotation().phi() , Angle::createFromDegree(0.1));
  EXPECT_NEAR(result.translation().x(), transform.translation().x(), 1e-3);
  EXPECT_NEAR(result.translation().y(), transform.translation().y(), 1e-3);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
