/**
 * Algorithm and estimation function regarding tsd.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#include <francor_base/laser_scan.h>
#include <francor_base/line.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/algorithm/tsd.h"

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

bool reconstructPointsFromGrid(const TsdGrid& grid, const base::Pose2d& pose, const base::Angle phi_min,
                               const base::Angle phi_step, const std::size_t num_beams, const double range,
                               base::Point2dVector& points)
{
  using francor::base::Point2d;
  using francor::base::Point2dVector;
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;

  Angle current_phi = pose.orientation() + phi_min;
  const std::size_t start_index_x = grid.getIndexX(pose.position().x());
  const std::size_t start_index_y = grid.getIndexY(pose.position().y());

  points.resize(0);
  points.reserve(num_beams);

  for (std::size_t beam = 0; beam < num_beams; ++beam)
  {
    const auto direction = base::algorithm::line::calculateV(current_phi);
    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(),
                            grid.getNumCellsY(), grid.getCellSize(), pose.position(), direction, range)); // \todo make distance adjustable

    for (const auto& idx : ray)
    {
      if (grid(idx.x(), idx.y()).tsd > 0.0) {
        points.push_back( { static_cast<double>(idx.x()) * grid.getCellSize() + grid.getOrigin().x(),
                            static_cast<double>(idx.y()) * grid.getCellSize() + grid.getOrigin().y() } );

        break;
      }
    }                            

    current_phi += phi_step;
  }

  return true;
}

bool convertTsdToOccupancyGrid(const TsdGrid& tsd_grid, OccupancyGrid& occupancy_grid)
{
  occupancy_grid.init(tsd_grid.getNumCellsX(), tsd_grid.getNumCellsY(), tsd_grid.getCellSize());

  assert(tsd_grid.getNumCellsX() == occupancy_grid.getNumCellsX());
  assert(tsd_grid.getNumCellsY() == occupancy_grid.getNumCellsY());
  assert(tsd_grid.getCellSize() == occupancy_grid.getCellSize());

  for (std::size_t y = 0; y < occupancy_grid.getNumCellsY(); ++y) {
    for (std::size_t x = 0; x < occupancy_grid.getNumCellsX(); ++x) {
      // \todo replace it with a proper formula
      if (tsd_grid(x, y).tsd > 0.0) {
        occupancy_grid(x, y).value = 100;
      }
      else {
        occupancy_grid(x, y).value = 0;
      }
    }
  }

  return true;
}

} // end namespace tsd

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor