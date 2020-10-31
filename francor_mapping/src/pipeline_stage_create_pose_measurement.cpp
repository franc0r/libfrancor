/**
 * Processing pipeline stages for ego object.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_create_pose_measurement.h"

#include <francor_base/transform.h>

namespace francor {

namespace mapping {

bool StageCreatePoseMeasurement::doProcess(processing::NoDataType&)
{
  using francor::base::LogDebug;
  
  const auto delta_pose = this->input(IN_DELTA_POSE).data<base::Transform2d>();
  const auto ego_pose   = this->input(IN_EGO_POSE).data<base::Pose2d>();
  const auto time_stamp = this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>()->timeStamp();
  LogDebug() << this->name() << ": create pose measurement using delta pose (" << delta_pose << ") and ego pose (" << ego_pose << ")";

  const auto estimated_pose = delta_pose * ego_pose;
  LogDebug() << this->name() << ": estimated pose = " << estimated_pose << ", valid for time stamp = " << time_stamp;

  // @todo add paramter for covariances
  base::Matrix3d covariances = base::Matrix3d::Zero();
  covariances(0, 0) = 0.5 * 0.5;
  covariances(1, 1) = 0.5 * 0.5;
  covariances(2, 2) = base::Angle::createFromDegree(10) * base::Angle::createFromDegree(10);
  *_sensor_data = base::PoseSensorData(time_stamp, estimated_pose, covariances, "localization");

  return true;
}

bool StageCreatePoseMeasurement::doInitialization()
{
  _sensor_data = std::make_shared<base::PoseSensorData>("localization");
  return true;
}

bool StageCreatePoseMeasurement::initializePorts()
{
  this->initializeInputPort<base::Transform2d>(IN_DELTA_POSE, "delta_pose");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego_pose");
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SENSOR_DATA, "sensor_data");

  this->initializeOutputPort(OUT_SENSOR_DATA, "sensor_data", &_sensor_data);

  return true;
}

bool StageCreatePoseMeasurement::validateInputData() const
{
  using francor::base::LogError;

  if (nullptr == this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>()) {
    LogError() << this->name() << ": input sensor data pointer is null";
    return false;
  }

  return true;
}

bool StageCreatePoseMeasurement::isReady() const
{
  return this->input(IN_DELTA_POSE).numOfConnections() > 0
         &&
         this->input(IN_EGO_POSE).numOfConnections() > 0
         &&
         this->input(IN_SENSOR_DATA).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor