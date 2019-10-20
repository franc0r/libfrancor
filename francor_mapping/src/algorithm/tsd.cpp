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


void pushLaserScanToGrid(TsdGrid& grid, const base::LaserScan& laser_scan, const base::Pose2d& pose_ego)
{
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
    std::size_t counter = 0;

    for (const auto& idx : ray)
    {
      const auto sdf = algorithm::tsd::calculateSdf(grid.getCellPosition(idx.x(), idx.y()), position, distance);
      algorithm::tsd::updateTsdCell(grid(idx.x(), idx.y()), sdf, max_truncation); // \todo replace constant value with tsd calculation function
      ++counter;
    }

    std::cout << "counter = " << counter << std::endl;
    current_phi += laser_scan.phiStep();
  }
}


} // end namespace tsd

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor