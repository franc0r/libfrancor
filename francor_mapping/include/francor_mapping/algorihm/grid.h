/**
 * Algorithm and estimation function regarding grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_base/laser_scan.h>
#include <francor_base/pose.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/grid.h"

namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

/**
 * \brief Pushes an laser scan to an grid. The laser scan pose will be transformed by ego pose.
 * 
 * \param grid The grid the laser scan will be pushed to.
 * \param scan The input laser scan.
 * \param pose_ego The pose of the ego object in frame map
 * \tparam GridCellType The type of each grid cell.
 * \tparam UpdateFunction The function used to update each grid cell.
 */
template <typename GridCellType, typename UpdateFunction>
void pushLaserScanToGrid(Grid<GridCellType>& grid, const base::LaserScan& scan, const base::Pose2d& pose_ego)
{
  static_assert(std::is_function(UpdateFunction), "The template argument \"UpdateFunction\" must be a function.");

  using francor::base::Point2d;
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;

  Angle current_phi = laser_scan.phiMin();
  const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x() + pose_ego.position().x());
  const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y() + pose_ego.position().y());
  const double max_truncation = grid.getMaxTruncation();

  for (const auto& distance : laser_scan.distances())
  {
    const Point2d position = laser_scan.pose().position() + pose_ego.position();
    const Angle phi = current_phi + laser_scan.pose().orientation() + pose_ego.orientation();
    const auto direction = base::algorithm::line::calculateV(phi);

    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(), grid.getNumCellsY(), grid.getCellSize(), position, direction, distance));

    for (const auto& idx : ray)
    {
      UpdateFunction(grid(idx.x(), idx.y()), )
      const auto sdf = algorithm::tsd::calculateSdf(grid.getCellPosition(idx.x(), idx.y()), position, distance);
      algorithm::tsd::updateTsdCell(grid(idx.x(), idx.y()), sdf, max_truncation); // \todo replace constant value with tsd calculation function
    }

    current_phi += laser_scan.phiStep();
  }
}

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor