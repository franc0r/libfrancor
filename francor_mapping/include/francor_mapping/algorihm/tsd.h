/**
 * Algorithm and estimation function regarding tsd.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include <francor_base/pose.h>

#include "francor_mapping/tsd_grid.h"

namespace francor {

namespace base {
class LaserScan;
}

namespace mapping {

namespace algorithm {

namespace tsd {


void pushLaserScanToGrid(TsdGrid& grid, const base::LaserScan& scan, const base::Pose2d& pose_ego);

/**
 * \brief Calculates the sdf value for TSD cells.
 * 
 * \param distance_cell_sensor Distance between the sensor and the grid cell in meter.
 * \param measurement The distance measurement from the sensor.
 * \return Calculated sdf value.
 */
inline double calculateSdf(const double distance_cell_sensor, const double measurement)
{
  return measurement - distance_cell_sensor;
}
/**
 * \brief Calculates the sdf value for TSD cells.
 * 
 * \param cell_position The position of the grid cell in meter.
 * \param sensor_position The position of the used sensor.
 * \param measurement The distance measurement from the sensor.
 * \return Calculated sdf value.
 */
inline double calculateSdf(const base::Point2d& cell_position, const base::Point2d& sensor_position, const double measurement)
{
  const double distance_cell_sensor = base::Vector2d(cell_position.x() - sensor_position.x(),
                                                     cell_position.y() - sensor_position.y()).norm();
  return calculateSdf(distance_cell_sensor, measurement);
}
/**
 * \brief Updates a tsd grid cell with given sdf value.
 * 
 * \param cell TsdGrid cell that will be updated.
 * \param sdf The sdf value used to update tsd cell.
 * \param max_truncation The maximal truncation value.
 */
inline void updateTsdCell(TsdCell& cell, const double sdf, const double max_truncation)
{
  double tsdf = sdf / max_truncation;
  std::min(tsdf, 1.0);

  cell.weight += 1.0;

  if (std::isnan(cell.tsd)) {
    cell.tsd = tsdf;
  }
  else {
    cell.weight = std::min(cell.weight, 200.0); // \todo check if limit of 200 is good.
    cell.tsd    = (cell.tsd * (cell.weight - 1.0) + tsdf) / cell.weight;
  }
}
} // end namespace tsd

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor