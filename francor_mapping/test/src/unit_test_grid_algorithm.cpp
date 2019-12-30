/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>
#include "francor_mapping/algorithm/grid.h"

TEST(PointDistribution, Create)
{
  using francor::mapping::algorithm::grid::point_distribution;

  using point3 = point_distribution<3, 3, float>;
  std::cout << "(" << point3::cell<0, 0>::value << ", " << point3::cell<1, 0>::value << ", " << point3::cell<2, 0>::value << ")" << std::endl;
  std::cout << "(" << point3::cell<0, 1>::value << ", " << point3::cell<1, 1>::value << ", " << point3::cell<2, 1>::value << ")" << std::endl;
  std::cout << "(" << point3::cell<0, 2>::value << ", " << point3::cell<1, 2>::value << ", " << point3::cell<2, 2>::value << ")" << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}