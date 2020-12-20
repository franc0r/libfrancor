#include <gtest/gtest.h>

#include "francor_mapping/grid.h"

using francor::mapping::Grid;

TEST(Grid, InstantiateEmptyGrid)
{
  Grid<double> grid;

  // check if all attributes are zero
  EXPECT_EQ(grid.cell().count().x(), 0);
  EXPECT_EQ(grid.cell().count().y(), 0);
  EXPECT_EQ(grid.cell().size(), 0.0);
  EXPECT_EQ(grid.size().x(), 0.0);
  EXPECT_EQ(grid.size().y(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid.isEmpty());
  EXPECT_FALSE(grid.isValid());
}

TEST(Grid, Initialize)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init({10u, 10u}, 1.0));

  // check all attributes
  EXPECT_EQ(grid.cell().count().x(), 10);
  EXPECT_EQ(grid.cell().count().y(), 10);
  EXPECT_EQ(grid.cell().size(), 1.0);
  EXPECT_NEAR(grid.size().x(), 10.0, 1e-6);
  EXPECT_NEAR(grid.size().y(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid.isValid());
  EXPECT_FALSE(grid.isEmpty());
}

TEST(Grid, AccessGridCells)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid.cell().count().y(); ++y)
      grid(x, y) = static_cast<double>(x * grid.cell().count().y() + y);

  // read back all values and compare
  for (std::size_t x = 0; x < grid.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid.cell().count().y(); ++y)
      EXPECT_NEAR(grid(x, y), static_cast<double>(x * grid.cell().count().y() + y), 1e-6);
}

TEST(Grid, CopyConstructor)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_origin.cell().count().y(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.cell().count().y() + y);

  // creat new grid as copy from origin
  Grid<double> grid_copy(grid_origin);

  // check all attributes
  EXPECT_EQ(grid_copy.cell().count().x(), 10);
  EXPECT_EQ(grid_copy.cell().count().y(), 10);
  EXPECT_EQ(grid_copy.cell().size(), 1.0);
  EXPECT_NEAR(grid_copy.size().x(), 10.0, 1e-6);
  EXPECT_NEAR(grid_copy.size().y(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_copy.isValid());
  EXPECT_FALSE(grid_copy.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_copy.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_copy.cell().count().y(); ++y)
      EXPECT_NEAR(grid_copy(x, y), static_cast<double>(x * grid_copy.cell().count().y() + y), 1e-6);
}

TEST(Grid, MoveConstructor)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_origin.cell().count().y(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.cell().count().y() + y);

  // creat new grid and move content from origin into
  Grid<double> grid_moved(std::move(grid_origin));

  // check all attributes
  EXPECT_EQ(grid_moved.cell().count().x(), 10);
  EXPECT_EQ(grid_moved.cell().count().y(), 10);
  EXPECT_EQ(grid_moved.cell().size(), 1.0);
  EXPECT_NEAR(grid_moved.size().x(), 10.0, 1e-6);
  EXPECT_NEAR(grid_moved.size().y(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_moved.isValid());
  EXPECT_FALSE(grid_moved.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_moved.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_moved.cell().count().y(); ++y)
      EXPECT_NEAR(grid_moved(x, y), static_cast<double>(x * grid_moved.cell().count().y() + y), 1e-6);

  // check if all attributes are cleared
  EXPECT_EQ(grid_origin.cell().count().x(), 0);
  EXPECT_EQ(grid_origin.cell().count().y(), 0);
  EXPECT_EQ(grid_origin.cell().size(), 0.0);
  EXPECT_EQ(grid_origin.size().x(), 0.0);
  EXPECT_EQ(grid_origin.size().y(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid_origin.isEmpty());
  EXPECT_FALSE(grid_origin.isValid());
}

TEST(Grid, Clear)
{
  Grid<double> grid;

  // initialize grid
  EXPECT_TRUE(grid.init({10u, 10u}, 1.0));

  // clear grid
  grid.clear();

  // check if all attributes are zero
  EXPECT_EQ(grid.cell().count().x(), 0);
  EXPECT_EQ(grid.cell().count().y(), 0);
  EXPECT_EQ(grid.cell().size(), 0.0);
  EXPECT_EQ(grid.size().x(), 0.0);
  EXPECT_EQ(grid.size().y(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid.isEmpty());
  EXPECT_FALSE(grid.isValid());
}

TEST(Grid, CopyAssignmentOperator)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_origin.cell().count().y(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.cell().count().y() + y);

  // creat new empty grid 
  Grid<double> grid_copy;
  
  // copy
  grid_copy = grid_origin;

  // check all attributes
  EXPECT_EQ(grid_copy.cell().count().x(), 10);
  EXPECT_EQ(grid_copy.cell().count().y(), 10);
  EXPECT_EQ(grid_copy.cell().size(), 1.0);
  EXPECT_NEAR(grid_copy.size().x(), 10.0, 1e-6);
  EXPECT_NEAR(grid_copy.size().y(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_copy.isValid());
  EXPECT_FALSE(grid_copy.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_copy.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_copy.cell().count().y(); ++y)
      EXPECT_NEAR(grid_copy(x, y), static_cast<double>(x * grid_copy.cell().count().y() + y), 1e-6);
}

TEST(Grid, MoveAssignmentOperator)
{
  Grid<double> grid_origin;

  // initialize origin
  EXPECT_TRUE(grid_origin.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid_origin.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_origin.cell().count().y(); ++y)
      grid_origin(x, y) = static_cast<double>(x * grid_origin.cell().count().y() + y);

  // creat new empty grid
  Grid<double> grid_moved;
  
  // move content
  grid_moved = std::move(grid_origin);

  // check all attributes
  EXPECT_EQ(grid_moved.cell().count().x(), 10);
  EXPECT_EQ(grid_moved.cell().count().y(), 10);
  EXPECT_EQ(grid_moved.cell().size(), 1.0);
  EXPECT_NEAR(grid_moved.size().x(), 10.0, 1e-6);
  EXPECT_NEAR(grid_moved.size().y(), 10.0, 1e-6);

  // grid must be valid and not empty
  EXPECT_TRUE(grid_moved.isValid());
  EXPECT_FALSE(grid_moved.isEmpty());  

  // check if all cells were copied right
  for (std::size_t x = 0; x < grid_moved.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid_moved.cell().count().y(); ++y)
      EXPECT_NEAR(grid_moved(x, y), static_cast<double>(x * grid_moved.cell().count().y() + y), 1e-6);

  // check if all attributes are cleared
  EXPECT_EQ(grid_origin.cell().count().x(), 0);
  EXPECT_EQ(grid_origin.cell().count().y(), 0);
  EXPECT_EQ(grid_origin.cell().size(), 0.0);
  EXPECT_EQ(grid_origin.size().x(), 0.0);
  EXPECT_EQ(grid_origin.size().y(), 0.0);

  // grid must be empty and invalid
  EXPECT_TRUE(grid_origin.isEmpty());
  EXPECT_FALSE(grid_origin.isValid()); 
}

TEST(Grid, DataAccessViaIterator)
{
  Grid<double> grid;

  // initialize origin
  EXPECT_TRUE(grid.init({10u, 10u}, 1.0));

  // set values to each grid cell using an counter
  for (std::size_t x = 0; x < grid.cell().count().x(); ++x)
    for (std::size_t y = 0; y < grid.cell().count().y(); ++y)
      grid(x, y) = static_cast<double>(x * grid.cell().count().y() + y);

  for (const auto& element : grid.row(0).all_elements()) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
  for (const auto& element : grid.row(0).every_second_element()) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
}


int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}