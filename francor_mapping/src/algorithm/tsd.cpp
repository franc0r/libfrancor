/**
 * Algorithm and estimation function regarding tsd.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#include <francor_base/laser_scan.h>
#include <francor_base/line.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/algorihm/tsd.h"

namespace francor {

namespace mapping {

namespace algorithm {

namespace tsd {


void pushLaserScanToGrid(TsdGrid& grid, const base::LaserScan& laser_scan)
{
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;

  Angle current_phi = laser_scan.phiMin();
  const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x());
  const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y());

  for (const auto& distance : laser_scan.distances())
  {
    const auto direction = Line(current_phi + laser_scan.pose().orientation()).v();
    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getCellSize(), laser_scan.pose().position(), direction, distance));

    for (const auto& idx : ray)
      grid(idx.x(), idx.y()) = { 1.0, 1.0 }; // \todo replace constant value with tsd calculation function

    current_phi += laser_scan.phiStep();
  }
}


} // end namespace tsd

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor