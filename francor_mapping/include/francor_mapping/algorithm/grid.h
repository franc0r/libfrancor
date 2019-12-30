/**
 * Algorithm and estimation function regarding grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_base/laser_scan.h>
#include <francor_base/log.h>
#include <francor_base/pose.h>
#include <francor_base/algorithm/math.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/grid.h"

namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

template <std::size_t SizeX, std::size_t SizeY, typename Type>
struct point_distribution
{
  using p = base::algorithm::math::floating_number<5, -1, Type>;
  template <std::size_t N>
  using distribution = base::algorithm::math::binomial_distribution<N, p, Type>;

  template <std::size_t X>
  struct x
  {
    static constexpr Type value = distribution<SizeX>::template pm<X>::value;
  };

  template <std::size_t Y>
  struct y
  {
    static constexpr Type value = distribution<SizeY>::template pm<Y>::value;
  };

  template <std::size_t X, std::size_t Y>
  struct cell
  {
    static constexpr Type value = x<X>::value * y<Y>::value;
  };
};

template<std::size_t SizeX, std::size_t SizeY, typename ValueType, class... DistributionFunction>
class UpdateMatrix
{
public:
  constexpr UpdateMatrix()
  { 
    // for (std::size_t row = 0; row < Rows; ++row) {
    //   for (std::size_t col = 0; col < Cols; ++col) {
    //     _data[row][col] = DistributionFunction(col, row);
    //   }
    // }
  }

   ValueType operator()(const std::size_t row, const std::size_t col) const
  {
            // std::cout << "data[" << row << ", " << col << "] = " << _data[row][col] << " ";
    return _data[row][col];
  }

private:
  ValueType _data[SizeX][SizeY] = { };
};

/**
 * \brief Pushes a point respecting its variance into a grid. The shape of the point is
 *        according gaussian distribution.
 * 
 * \param grid Occupancy grid.
 * \param center_x X coordinate on grid.
 * \param center_y y coordinate on grid.
 * \param point_size Size of the laser point in grid cells.
 * \tparam GridCellType The type of each grid cell.
 * \tparam UpdateFunction The function used to update each grid cell.
 */
template <typename GridType, typename UpdateFunction>
void pushPoint(GridType& grid, const std::size_t center_x, const std::size_t center_y, const std::size_t point_size)
{
  assert(point_size % 2 == 1);
  const int side_size = point_size / 2;

  // do nothing if point is out of range including point size
  if (static_cast<int>(center_x) - side_size < 0 || static_cast<int>(center_x) + side_size >= grid.getNumCellsX()) {
    return;
  }
  if (static_cast<int>(center_y) - side_size < 0 || static_cast<int>(center_y) + side_size >= grid.getNumCellsY()) {
    return;
  }

  switch (point_size)
  {
  // case 1:
  //   {
  //     constexpr UpdateMatrix<1, 1, float, PointDistribution<1, 1, float>> update_matrix;
  //     UpdateFunction(grid(center_x, center_y), update_matrix(0, 0));
  //   }
  //   break;
  
  // case 3:
  //   {
  //     constexpr UpdateMatrix<3, 3, float, PointDistribution<3, 3, float>> update_matrix;      
  //     for (std::size_t x = 0; x < point_size; ++x) {
  //       for (std::size_t y = 0; y < point_size; ++y) {
  //         UpdateFunction(grid(center_x, center_y), update_matrix(y, x));
  //       }
  //     }
  // }
  // break;

  // case 5:
  //   {
  //     constexpr UpdateMatrix<5, 5, float, PointDistribution<5, 5, float>> update_matrix;      
  //     for (std::size_t x = 0; x < point_size; ++x) {
  //       for (std::size_t y = 0; y < point_size; ++y) {
  //         UpdateFunction(grid(center_x, center_y), update_matrix(y, x));
  //       }
  //     }
  //   }
  //   break;

  // case 7:
  //   {
  //     constexpr UpdateMatrix<7, 7, float, PointDistribution<7, 7, float>> update_matrix;      
  //     for (std::size_t x = 0; x < point_size; ++x) {
  //       for (std::size_t y = 0; y < point_size; ++y) {
  //         UpdateFunction(grid(center_x, center_y), update_matrix(y, x));
  //       }
  //     }
  //   }
  //   break;

  default:
    francor::base::LogError() << "pushPoint(): point size = " << point_size << " isn't supported.";
    break;
  }
}

/**
 * \brief Pushes an laser scan to an grid. The laser scan pose will be transformed by ego pose.
 * 
 * \param grid The grid the laser scan will be pushed to.
 * \param scan The input laser scan.
 * \param pose_ego The pose of the ego object in frame map
 * \tparam GridCellType The type of each grid cell.
 * \tparam UpdateFunction The function used to update each grid cell.
 */
// template <typename GridCellType, typename UpdateFunction>
// void pushLaserScanToGrid(Grid<GridCellType>& grid, const base::LaserScan& scan, const base::Pose2d& pose_ego)
// {
//   static_assert(std::is_function(UpdateFunction), "The template argument \"UpdateFunction\" must be a function.");

//   using francor::base::Point2d;
//   using francor::base::Angle;
//   using francor::base::Line;
//   using francor::algorithm::Ray2d;

//   Angle current_phi = laser_scan.phiMin();
//   const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x() + pose_ego.position().x());
//   const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y() + pose_ego.position().y());
//   const double max_truncation = grid.getMaxTruncation();

//   for (const auto& distance : laser_scan.distances())
//   {
//     const Point2d position = laser_scan.pose().position() + pose_ego.position();
//     const Angle phi = current_phi + laser_scan.pose().orientation() + pose_ego.orientation();
//     const auto direction = base::algorithm::line::calculateV(phi);

//     Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(), grid.getNumCellsY(), grid.getCellSize(), position, direction, distance));

//     for (const auto& idx : ray)
//     {
//       UpdateFunction(grid(idx.x(), idx.y()), )
//       const auto sdf = algorithm::tsd::calculateSdf(grid.getCellPosition(idx.x(), idx.y()), position, distance);
//       algorithm::tsd::updateTsdCell(grid(idx.x(), idx.y()), sdf, max_truncation); // \todo replace constant value with tsd calculation function
//     }

//     current_phi += laser_scan.phiStep();
//   }
// }

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor