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

template <std::size_t SizeX, std::size_t SizeY, typename Type = float>
class PointDistribution
{
public:
  constexpr PointDistribution()
  {
    using base::algorithm::math::BinomialDistribution;
    constexpr BinomialDistribution<SizeX - 1, Type> distribution_x(0.5);
    constexpr BinomialDistribution<SizeY - 1, Type> distribution_y(0.5);

    for (std::size_t x = 0; x < SizeX; ++x) {
      for (std::size_t y = 0; y < SizeY; ++y) {
        _data[x][y] = distribution_x.pm(x) * distribution_y.pm(y) + static_cast<Type>(0.5);
      }
    }
  }

  inline constexpr Type operator()(const std::size_t x, const std::size_t y) const noexcept { return _data[x][y]; }

private:
  Type _data[SizeX][SizeY] = { };
};

template<std::size_t SizeX, std::size_t SizeY, typename ValueType, class Distributions>
class UpdateMatrix
{
public:
  constexpr UpdateMatrix()
  { 

  }

  constexpr ValueType operator()(const std::size_t row, const std::size_t col) const
  {
            // std::cout << "data[" << row << ", " << col << "] = " << _data[row][col] << " ";
    return { };
  }
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
  if (static_cast<int>(center_x) - static_cast<int>(side_size) < 0 || center_x + side_size >= grid.getNumCellsX()) {
    return;
  }
  if (static_cast<int>(center_y) - static_cast<int>(side_size) < 0 || center_y + side_size >= grid.getNumCellsY()) {
    return;
  }

  switch (point_size)
  {
  case 1:
    {
      UpdateFunction(grid(center_x, center_y), 0.95);
    }
    break;
  
  case 3:
    {
      constexpr PointDistribution<3, 3, float> update_matrix;      
      for (std::size_t x = 0; x < point_size; ++x) {
        for (std::size_t y = 0; y < point_size; ++y) {
          UpdateFunction(grid(center_x - 1 + x, center_y - 1 + y), update_matrix(y, x));
        }
      }
  }
  break;

  case 5:
    {
      constexpr PointDistribution<5, 5, float> update_matrix;      
      for (std::size_t x = 0; x < point_size; ++x) {
        for (std::size_t y = 0; y < point_size; ++y) {
          UpdateFunction(grid(center_x - 2 + x, center_y - 2 + y), update_matrix(y, x));
        }
      }
    }
    break;

  case 7:
    {
      constexpr PointDistribution<7, 7, float> update_matrix;      
      for (std::size_t x = 0; x < point_size; ++x) {
        for (std::size_t y = 0; y < point_size; ++y) {
          UpdateFunction(grid(center_x - 3 + x, center_y - 3 + y), update_matrix(y, x));
        }
      }
    }
    break;

    case 9:
    {
      constexpr PointDistribution<9, 9, float> update_matrix;      
      for (std::size_t x = 0; x < point_size; ++x) {
        for (std::size_t y = 0; y < point_size; ++y) {
          UpdateFunction(grid(center_x - 4 + x, center_y - 4 + y), update_matrix(y, x));
        }
      }
    }
    break;

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