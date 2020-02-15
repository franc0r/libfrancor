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

TEST(Grid, CopyConstructor)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init(10u, 10u, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_origin.getNumCellsY(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.getNumCellsY() + y);

  // creat new grid as copy from origin
  Grid<double> grid_copy(grid_origin);

  // check all attributes
  EXPECT_EQ(grid_copy.getNumCellsX(), 10);
  EXPECT_EQ(grid_copy.getNumCellsY(), 10);
  EXPECT_EQ(grid_copy.getCellSize(), 1.0);
  EXPECT_NEAR(grid_copy.getSizeX(), 10.0, 1e-6);
  EXPECT_NEAR(grid_copy.getSizeY(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_copy.isValid());
  EXPECT_FALSE(grid_copy.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_copy.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_copy.getNumCellsY(); ++y)
      EXPECT_NEAR(grid_copy(x, y), static_cast<double>(x * grid_copy.getNumCellsY() + y), 1e-6);
}

TEST(Grid, MoveConstructor)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init(10u, 10u, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_origin.getNumCellsY(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.getNumCellsY() + y);

  // creat new grid and move content from origin into
  Grid<double> grid_moved(std::move(grid_origin));

  // check all attributes
  EXPECT_EQ(grid_moved.getNumCellsX(), 10);
  EXPECT_EQ(grid_moved.getNumCellsY(), 10);
  EXPECT_EQ(grid_moved.getCellSize(), 1.0);
  EXPECT_NEAR(grid_moved.getSizeX(), 10.0, 1e-6);
  EXPECT_NEAR(grid_moved.getSizeY(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_moved.isValid());
  EXPECT_FALSE(grid_moved.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_moved.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_moved.getNumCellsY(); ++y)
      EXPECT_NEAR(grid_moved(x, y), static_cast<double>(x * grid_moved.getNumCellsY() + y), 1e-6);

  // check if all attributes are cleared
  EXPECT_EQ(grid_origin.getNumCellsX(), 0);
  EXPECT_EQ(grid_origin.getNumCellsY(), 0);
  EXPECT_EQ(grid_origin.getCellSize(), 0.0);
  EXPECT_EQ(grid_origin.getSizeX(), 0.0);
  EXPECT_EQ(grid_origin.getSizeY(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid_origin.isEmpty());
  EXPECT_FALSE(grid_origin.isValid());
}

TEST(Grid, Clear)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init(10u, 10u, 1.0));

  // clear grid
  grid.clear();

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

TEST(Grid, CopyAssignmentOperator)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init(10u, 10u, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_origin.getNumCellsY(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.getNumCellsY() + y);

  // creat new empty grid 
  Grid<double> grid_copy;
  
  // copy
  grid_copy = grid_origin;

  // check all attributes
  EXPECT_EQ(grid_copy.getNumCellsX(), 10);
  EXPECT_EQ(grid_copy.getNumCellsY(), 10);
  EXPECT_EQ(grid_copy.getCellSize(), 1.0);
  EXPECT_NEAR(grid_copy.getSizeX(), 10.0, 1e-6);
  EXPECT_NEAR(grid_copy.getSizeY(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_copy.isValid());
  EXPECT_FALSE(grid_copy.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_copy.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_copy.getNumCellsY(); ++y)
      EXPECT_NEAR(grid_copy(x, y), static_cast<double>(x * grid_copy.getNumCellsY() + y), 1e-6);
}

TEST(Grid, MoveAssignmentOperator)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init(10u, 10u, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_origin.getNumCellsY(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.getNumCellsY() + y);

  // creat new empty grid
  Grid<double> grid_moved;
  
  // move content
  grid_moved = std::move(grid_origin);

  // check all attributes
  EXPECT_EQ(grid_moved.getNumCellsX(), 10);
  EXPECT_EQ(grid_moved.getNumCellsY(), 10);
  EXPECT_EQ(grid_moved.getCellSize(), 1.0);
  EXPECT_NEAR(grid_moved.getSizeX(), 10.0, 1e-6);
  EXPECT_NEAR(grid_moved.getSizeY(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_moved.isValid());
  EXPECT_FALSE(grid_moved.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_moved.getNumCellsX(); ++x)
    for (std::size_t y = 0; y < grid_moved.getNumCellsY(); ++y)
      EXPECT_NEAR(grid_moved(x, y), static_cast<double>(x * grid_moved.getNumCellsY() + y), 1e-6);

  // check if all attributes are cleared
  EXPECT_EQ(grid_origin.getNumCellsX(), 0);
  EXPECT_EQ(grid_origin.getNumCellsY(), 0);
  EXPECT_EQ(grid_origin.getCellSize(), 0.0);
  EXPECT_EQ(grid_origin.getSizeX(), 0.0);
  EXPECT_EQ(grid_origin.getSizeY(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid_origin.isEmpty());
  EXPECT_FALSE(grid_origin.isValid()); 
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}