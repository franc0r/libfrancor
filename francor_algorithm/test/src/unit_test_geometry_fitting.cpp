/**
 * Unit test for the geometry fitting functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 28. June 2020
 */
#include <gtest/gtest.h>

#include <francor_base/point.h>

#include <francor_algorithm/geometry_fitting.h>

TEST(EstimateNormalsFromOrderedPoints, BasicFunctionality)
{
  using francor::algorithm::estimateNormalsFromOrderedPoints;
  using francor::base::Point2d;
  using francor::base::Point2dVector;

  const Point2dVector point_set = { { 2.0, 2.0 }, { 1.0, 2.0 }, { 0.0, 2.0 }, { -1.0, 2.0 }, { -2.0, 2.0 },
                                    { -2.0, 1.0 }, { -2.0, 0.0 }, { -2.0, -1.0 }, { -2.0, -2.0 }};
  auto ret = estimateNormalsFromOrderedPoints(point_set, 3);

  ASSERT_TRUE(ret) << "function returned false";

  for (const auto normal : *ret) {
    std::cout << normal << std::endl;                                    
  }
}