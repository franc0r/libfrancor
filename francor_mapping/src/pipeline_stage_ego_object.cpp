/**
 * Processing pipeline stages for ego object.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_ego_object.h"
#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"
#include "francor_mapping/ego_motion_sensor_model.h"
#include "francor_mapping/pose_sensor_model.h"

#include <francor_base/laser_scan.h>
#include <francor_base/transform.h>

namespace francor {

namespace mapping {

bool StageEstimateLaserScannerPose::doProcess(EgoObject& ego)
{
  using francor::base::LogDebug;
  using francor::base::LogError;
  
  const auto pose(this->input(IN_SENSOR_POSE).numOfConnections() > 0   ?
                  this->input(IN_SENSOR_POSE).data<base::Pose2d>()     :
                  this->input(IN_SCAN).data<base::LaserScan>().pose());

  base::Transform2d t_laser_ego({ pose.orientation() }, { pose.position().x(), pose.position().y() });
  _estimated_pose = t_laser_ego * ego.pose();
  LogDebug() << this->name() << ": estimated " << _estimated_pose;

  const auto time_stamp = this->input(IN_SCAN).data<base::LaserScan>().timeStamp();

  if (ego.timeStamp() < time_stamp) {
    LogDebug() << this->name() << ": predict ego object state to time = " << time_stamp;
    KalmanFilter<EgoKalmanFilterModel> kalman_filter(ego.timeStamp(), ego.stateVector(), ego.covariances());
    
    if (false == kalman_filter.predictToTime(time_stamp)) {
      LogError() << this->name() << ": time prediction of ego object failed.";
      return false;
    }
  }

  _ego_pose = ego.pose();

  return true;
}

bool StageEstimateLaserScannerPose::doInitialization()
{
  return true;
}

bool StageEstimateLaserScannerPose::initializePorts()
{
  this->initializeInputPort<base::LaserScan>(IN_SCAN       , "laser scan" );
  this->initializeInputPort<base::Pose2d>   (IN_SENSOR_POSE, "sensor pose");

  this->initializeOutputPort(OUT_POSE, "pose", &_estimated_pose);
  this->initializeOutputPort(OUT_EGO_POSE, "ego pose", &_ego_pose);

  return true;
}

bool StageEstimateLaserScannerPose::isReady() const
{
  return this->input(IN_SCAN).numOfConnections() > 0
         ||
         this->input(IN_SENSOR_POSE).numOfConnections() > 0;
}



bool StageUpdateEgo::doProcess(EgoObject& ego)
{
  using francor::base::LogDebug;

  base::Transform2d transform = this->input(IN_TRANSFORM).data<base::Transform2d>();

  LogDebug() << this->name() << ": update ego pose = " << ego.pose();
  LogDebug() << this->name() << ": new ego pose = " << ego.pose();

  return true;
}

bool StageUpdateEgo::doInitialization()
{
  return true;
}

bool StageUpdateEgo::initializePorts()
{
  this->initializeInputPort<base::Pose2d>     (IN_SENSOR_POSE, "sensor pose");
  this->initializeInputPort<base::Transform2d>(IN_TRANSFORM  , "transform"  );

  return true;
}

bool StageUpdateEgo::isReady() const
{
  return this->input(IN_TRANSFORM).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor