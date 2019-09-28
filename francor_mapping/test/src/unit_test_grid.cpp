#include <gtest/gtest.h>

#include "francor_mapping/grid.h"

using francor::mapping::Grid;

TEST(Grid, InstantiateEmptyGrid)
{
  Grid<double> grid;

  // check if all attributes are zero
  EXPECT_EQ(grid.getNumCellsX(), 0);
  EXPECT_EQ(grid.getNumCellsY(), 0);
  EXPECT_EQ(grid.getCellSize(), 0.0);
  EXPECT_EQ(grid.getSizeX(), 0.0);
  EXPECT_EQ(grid.getSizeY(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid.isEmpty());
  EXPECT_FALSE(grid.isValid());
}

TEST(Grid, Initialize)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init(10u, 10u, 1.0));

  // check all attributes
  EXPECT_EQ(grid.getNumCellsX(), 10);
  EXPECT_EQ(grid.getNumCellsY(), 10);
  EXPECT_EQ(grid.getCellSize(), 1.0);
  EXPECT_NEAR(grid.getSizeX(), 10.0, 1e-6);
  EXPECT_NEAR(grid.getSizeY(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid.isValid());
  EXPECT_FALSE(grid.isEmpty());
}

TEST(Grid, AccessGridCells)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init(10u, 10u, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid.getNumCellsY(); ++y)
      grid(x, y) = static_cast<double>(x * grid.getNumCellsY() + y);

  // read back all values and compare
  for (std::size_t x = 0; x < grid.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid.getNumCellsY(); ++y)
      EXPECT_NEAR(grid(x, y), static_cast<double>(x * grid.getNumCellsY() + y), 1e-6);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}