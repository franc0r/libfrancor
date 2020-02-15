/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>

#include <francor_base/angle.h>

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"

TEST(OccupancyGridAlgorithm, PushPoint)
{
  using francor::mapping::OccupancyGrid;
  using francor::mapping::algorithm::occupancy::pushLaserPointToGrid;

  OccupancyGrid grid;

  ASSERT_TRUE(grid.init(10, 10, 0.1));

  std::cout << grid << std::endl;
      pushLaserPointToGrid(grid, 5, 5, 7, francor::base::Angle::createFromDegree(0.0));

  // for (std::size_t x = 0; x < grid.getNumCellsX(); ++x) {
    // for (std::size_t y = 0; y < 1; ++y) {
      // pushLaserPointToGrid(grid, x, x, 5, francor::base::Angle::createFromDegree(-45.0));
    // }
  // }
  
  std::cout << "after update:" << std::endl;
  std::cout << grid << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}