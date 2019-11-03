/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_occupancy_grid.h"

#include <francor_base/pose.h>

namespace francor {

namespace mapping {

bool PipeSimulateLaserScan::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(StageEstimateLaserScannerPose::IN_SENSOR_POSE)
                             .connect(this->input(IN_SENSOR_POSE));

  ret &= std::get<1>(_stages).input(StageReconstructPointsFromOccupancyGrid::IN_SENSOR_POSE)
                             .connect(std::get<0>(_stages).output(StageEstimateLaserScannerPose::OUT_POSE));                             
  ret &= std::get<1>(_stages).output(StageReconstructPointsFromOccupancyGrid::OUT_POINTS)
                             .connect(this->output(OUT_POINTS));

  return ret;                                                          
}

bool PipeSimulateLaserScan::initializePorts()
{
  this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor pose");

  this->initializeOutputPort<base::Point2dVector>(OUT_POINTS, "points 2d");

  return true;
}

} // end namespace mapping

} // end namespace francor