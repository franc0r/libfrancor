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
  using francor::mapping::algorithm::grid::PointDistribution;
  constexpr PointDistribution<3, 3, float> point3;
  
  for (std::size_t x = 0; x < 3; ++x) {
    for (std::size_t y = 0; y < 3; ++y)
      std::cout << point3(x, y) << " ";

    std::cout << std::endl;
  }

  constexpr PointDistribution<3, 6, float> point3x6;
  for (std::size_t x = 0; x < 3; ++x) {
    for (std::size_t y = 0; y < 6; ++y)
      std::cout << point3x6(x, y) << " ";

    std::cout << std::endl;
  }

  constexpr PointDistribution<1, 1, float> point;
  std::cout << point(0, 0) << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}