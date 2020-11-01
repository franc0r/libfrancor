/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>
#include "francor_mapping/algorithm/grid.h"

#include <francor_vision/image.h>
#include <francor_vision/io.h>

using francor::mapping::algorithm::grid::markLaserBeamBorder;
using francor::mapping::algorithm::grid::fillMarkedShapes;
using francor::mapping::algorithm::grid::registerLaserBeam;
using francor::mapping::Grid;
using francor::base::Angle;
using francor::base::AnglePiToPi;
using francor::vision::Image;
using francor::vision::ColourSpace;

void convertGridToImage(const Grid<std::uint8_t>& grid, Image& image)
{
  constexpr std::uint8_t pixel_value_unkown = 200;

  image.resize(grid.getNumCellsX(), grid.getNumCellsY(), ColourSpace::GRAY);

  for (std::size_t col = 0; col < image.cols(); ++col) {
    for (std::size_t row = 0; row < image.rows(); ++row) {
      const auto cell_value = grid(col, row);

      if (0 == cell_value) {
        image(row, col).gray() = pixel_value_unkown;
      }
      else if (cell_value <= 10) {
        image(row, col).gray() = 255;
      }
      else {
        image(row, col).gray() = (100 - cell_value) * 2;
      }
    }
  }
}

void showGrid(const Grid<std::uint8_t>& grid)
{
  Image out_grid;
  convertGridToImage(grid, out_grid);

  cv::Mat image;
  cv::resize(out_grid.cvMat(), image, cv::Size(1000, 1000));
  cv::imshow("grid", image);
  cv::waitKey(0);
}

TEST(markLaserBeamBorder, Function)
{
  Grid<std::uint8_t> grid;

  ASSERT_TRUE(grid.init(100, 100, 0.1, 0));

  // mark laser beam border
  const auto origin = grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2);
  AnglePiToPi phi(AnglePiToPi::createFromDegree(0.0));
  const Angle divergence(Angle::createFromDegree(5.0));
  const double distance = grid.getCellSize() * static_cast<double>(grid.getNumCellsX()) * 0.4;
  constexpr std::uint8_t value_free = 10;
  constexpr std::uint8_t value_occupied = 90;

  for (std::size_t i = 0; i < 16; ++i, phi += Angle::createFromDegree(22.5)) {
    markLaserBeamBorder(grid, origin, phi, divergence, distance, value_free, value_occupied);
  }
  // showGrid(grid);
}

TEST(fillMarkedShapes, Function)
{
  Grid<std::uint8_t> grid;

  ASSERT_TRUE(grid.init(100, 100, 0.1, 0));

  // mark laser beam border
  const auto origin = grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2);
  AnglePiToPi phi(AnglePiToPi::createFromDegree(0.0));
  const Angle divergence(Angle::createFromDegree(5.0));
  const double distance = grid.getCellSize() * static_cast<double>(grid.getNumCellsX()) * 0.4;
  constexpr std::uint8_t value_free = 10;
  constexpr std::uint8_t value_occupied = 90;

  for (std::size_t i = 0; i < 16; ++i, phi += Angle::createFromDegree(22.5)) {
    markLaserBeamBorder(grid, origin, phi, divergence, distance, value_free, value_occupied);
  }

  fillMarkedShapes(grid, value_occupied);

  // showGrid(grid);
}

TEST(registerLaserBeam, Function)
{
  Grid<std::uint8_t> grid;

  ASSERT_TRUE(grid.init(100, 100, 0.1, 0));

  // mark laser beam border
  const auto origin = grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2);
  AnglePiToPi phi(AnglePiToPi::createFromDegree(0.0));
  const Angle divergence(Angle::createFromDegree(5.0));
  const double distance = grid.getCellSize() * static_cast<double>(grid.getNumCellsX()) * 0.4;
  constexpr std::uint8_t value_free = 10;
  constexpr std::uint8_t value_occupied = 90;

  for (std::size_t i = 0; i < 16; ++i, phi += Angle::createFromDegree(22.5)) {
    registerLaserBeam(grid, origin, phi, divergence, distance, value_free, value_occupied);
  }

  showGrid(grid);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}