/**
 * Processing pipeline stages for ego object.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_ego_object.h"

#include <francor_base/laser_scan.h>
#include <francor_base/transform.h>

namespace francor {

namespace mapping {

bool StageEstimateLaserScannerPose::doProcess(EgoObject& ego)
{
  using francor::base::LogDebug;
  
  const auto pose_laser(this->input(IN_SCAN).data<base::LaserScan>().pose());

  base::Transform2d t_laser_ego({ pose_laser.orientation() }, { pose_laser.position().x(), pose_laser.position().y() });
  _estimated_pose = t_laser_ego * ego.pose();
  LogDebug() << this->name() << ": estimated " << _estimated_pose;

  return true;
}

bool StageEstimateLaserScannerPose::doInitialization()
{
  return true;
}

bool StageEstimateLaserScannerPose::initializePorts()
{
  this->initializeInputPort<base::LaserScan>(IN_SCAN, "laser scan");

  this->initializeOutputPort(OUT_POSE, "pose", &_estimated_pose);

  return true;
}

bool StageEstimateLaserScannerPose::isReady() const
{
  return this->input(IN_SCAN).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor