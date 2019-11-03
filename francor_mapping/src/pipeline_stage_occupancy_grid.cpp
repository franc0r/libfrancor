/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_occupancy_grid.h"
#include "francor_mapping/algorihm/occupancy_grid.h"

#include <francor_base/pose.h>

namespace francor {

namespace mapping {

bool StageReconstructPointsFromOccupancyGrid::doProcess(OccupancyGrid& grid)
{
  using francor::base::LogError;
  using francor::base::LogDebug;

  const auto& sensor_pose = this->input(IN_SENSOR_POSE).data<base::Pose2d>();
  LogDebug() << this->name() << ": start processing.";
  LogDebug() << this->name() << ": uses sensor pose = " << sensor_pose;

  if (!algorithm::occupancy::reconstructPointsFromGrid(grid,
                                                       sensor_pose,
                                                       _parameter.phi_min,
                                                       _parameter.phi_step,
                                                       _parameter.num_laser_beams,
                                                       _parameter.max_range,
                                                       _reconstructed_points))
  {
    LogError() << this->name() << ": reconstruct points from tsd grid failed.";
    return false;
  }              

  LogDebug() << this->name() << ": end processing.";
  return true;                                    
}

bool StageReconstructPointsFromOccupancyGrid::doInitialization()
{
  return true;
}

bool StageReconstructPointsFromOccupancyGrid::initializePorts()
{
  this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor pose");

  this->initializeOutputPort(OUT_POINTS, "points 2d", &_reconstructed_points);

  return true;
}

bool StageReconstructPointsFromOccupancyGrid::isReady() const
{
  return this->input(IN_SENSOR_POSE).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor