/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_occupancy_grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"

#include <francor_base/pose.h>

namespace francor {

namespace mapping {

bool StageReconstructPointsFromOccupancyGrid::doProcess(OccupancyGrid& grid)
{
  using francor::base::LogError;
  using francor::base::LogDebug;

  const auto& sensor_pose = this->input(IN_SENSOR_POSE).data<base::Pose2d>();
  const auto& ego_pose = this->input(IN_EGO_POSE).data<base::Pose2d>();
  const base::Pose2d origin(ego_pose.position() + sensor_pose.position(),
                            ego_pose.orientation() + sensor_pose.orientation()); 

  LogDebug() << this->name() << ": start processing.";
  LogDebug() << this->name() << ": uses sensor pose = " << origin;

  if (!algorithm::occupancy::reconstructPointsFromGrid(grid,
                                                       origin,
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
  this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor_pose");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego_pose");

  this->initializeOutputPort(OUT_POINTS, "points_2d", &_reconstructed_points);

  return true;
}

bool StageReconstructPointsFromOccupancyGrid::isReady() const
{
  return this->input(IN_SENSOR_POSE).numOfConnections() > 0;
}


bool StageReconstructLaserScanFromOccupancyGrid::doProcess(OccupancyGrid& grid)
{
  using francor::base::LogError;
  using francor::base::LogDebug;

  const auto& sensor_pose = this->input(IN_SENSOR_POSE).data<base::Pose2d>();
  const auto& ego_pose    = this->input(IN_EGO_POSE   ).data<base::Pose2d>();
  const auto  time_stamp  = this->input(IN_TIME_STAMP ).data<double      >();

  LogDebug() << this->name() << ": start processing.";
  LogDebug() << this->name() << ": uses sensor pose = " << sensor_pose << " and ego pose = " << ego_pose
             << " at time stamp = " << time_stamp;

  if (!algorithm::occupancy::reconstructLaserScanFromGrid(grid,
                                                          ego_pose,
                                                          sensor_pose,
                                                          _parameter.phi_min,
                                                          _parameter.phi_step,
                                                          _parameter.num_laser_beams,
                                                          _parameter.max_range,
                                                          _reconstructed_scan,
                                                          time_stamp))
  {
    LogError() << this->name() << ": reconstruct points from tsd grid failed.";
    return false;
  }              

  LogDebug() << this->name() << ": end processing.";
  return true;                                    
}

bool StageReconstructLaserScanFromOccupancyGrid::doInitialization()
{
  return true;
}

bool StageReconstructLaserScanFromOccupancyGrid::initializePorts()
{
  this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor pose");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego pose");
  this->initializeInputPort<double>(IN_TIME_STAMP, "time_stamp");

  this->initializeOutputPort(OUT_SCAN, "laser scan", &_reconstructed_scan);

  return true;
}

bool StageReconstructLaserScanFromOccupancyGrid::isReady() const
{
  return this->input(IN_SENSOR_POSE).numOfConnections() > 0
         &&
         this->input(IN_EGO_POSE).numOfConnections() > 0
         &&
         this->input(IN_TIME_STAMP).numOfConnections() > 0;
}



bool StagePushLaserScanToOccupancyGrid::doProcess(OccupancyGrid& grid)
{
  using francor::base::LogError;
  using francor::base::LogDebug;

  const auto& pose_ego = this->input(IN_EGO_POSE).data<base::Pose2d   >();
  const auto& scan     = this->input(IN_SCAN    ).data<base::LaserScan>();

  if (this->input(IN_NORMALS).numOfConnections() > 0) {
    const auto& normals = this->input(IN_NORMALS).data<std::vector<base::AnglePiToPi>>();
    LogDebug() << "Normals (size = " << normals.size() << ") will be used.";
    algorithm::occupancy::pushLaserScanToGrid(grid, scan, pose_ego, normals);
  }
  else {
    algorithm::occupancy::pushLaserScanToGrid(grid, scan, pose_ego);
  }

  return true;
}

bool StagePushLaserScanToOccupancyGrid::doInitialization()
{
  return true;
}

bool StagePushLaserScanToOccupancyGrid::initializePorts()
{
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego pose"  );
  this->initializeInputPort<base::LaserScan>(IN_SCAN, "laser scan");
  this->initializeInputPort<std::vector<base::AnglePiToPi>>(IN_NORMALS, "normals");

  return true;
}

bool StagePushLaserScanToOccupancyGrid::isReady() const
{
  return this->input(IN_EGO_POSE).numOfConnections() > 0
         &&
         this->input(IN_SCAN).numOfConnections() > 0;
}



bool StagePushPointsToOccupancyGrid::doProcess(OccupancyGrid& grid)
{
  // const auto& pose_ego = this->input(IN_EGO_POSE).data<base::Pose2d>();
  // const auto& points   = this->input(IN_POINTS).data<base::Point2dVector>();
  // const auto& normals  = this->input(IN_NORMALS).data<std::vector<base::AnglePiToPi>>();

  // algorithm::occupancy::
  // \todo finish implementation
  return false;
}

bool StagePushPointsToOccupancyGrid::doInitialization()
{
  return true;
}

bool StagePushPointsToOccupancyGrid::initializePorts()
{
  this->initializeInputPort<base::Point2d                     >(IN_EGO_POSE, "ego pose" );
  this->initializeInputPort<base::Point2dVector               >(IN_POINTS  , "points 2d");
  this->initializeInputPort<std::vector<base::AnglePiToPi>>(IN_NORMALS , "normals"  );

  return true;
}

bool StagePushPointsToOccupancyGrid::isReady() const
{
  return this->input(IN_EGO_POSE).numOfConnections() > 0
         &&
         this->input(IN_POINTS).numOfConnections() > 0
         &&
         this->input(IN_NORMALS).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor