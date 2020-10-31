/**
 * Processing pipeline stages for estimation of transformation between two data sets.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 9. November 2019
 */

#include "francor_algorithm/pipeline_stage_estimate_transform.h"
#include "francor_algorithm/geometry_fitting.h"

#include <francor_base/laser_scan.h>
#include <francor_base/algorithm/point.h>
#include <francor_base/log.h>

namespace francor {

namespace algorithm {

bool StageEstimateTransformBetweenPoints::doProcess(processing::NoDataType&)
{
  using francor::base::LogDebug;
  using francor::base::LogError;

  const auto& point_set_a = this->input(IN_POINTS_A).data<base::Point2dVector>();
  const auto& point_set_b = this->input(IN_POINTS_B).data<base::Point2dVector>();
  LogDebug() << this->name() << ": input points";
  LogDebug() << point_set_a;
  LogDebug() << point_set_b;

  // estimate transform between point sets using icp
  if (!_icp.estimateTransform(point_set_a, point_set_b, _estimated_transform)) {
    LogError() << this->name() << ": error occurred during estimation. Can't estimate transformatin.";
    return false;
  }

  _estimated_transform = _estimated_transform.inverse();
  LogDebug() << this->name() << ": estimated transform = " << _estimated_transform;
  return true;
}

bool StageEstimateTransformBetweenPoints::doInitialization()
{
  _icp.setMaxIterations(_parameter.max_iterations);
  _icp.setMaxRms(_parameter.max_rms);
  _icp.setTerminationRms(_parameter.termination_rms);

  return true;
}

bool StageEstimateTransformBetweenPoints::initializePorts()
{
  this->initializeInputPort<base::Point2dVector>(IN_POINTS_A, "points 2d");
  this->initializeInputPort<base::Point2dVector>(IN_POINTS_B, "points 2d");

  this->initializeOutputPort(OUT_TRANSFORM, "transform", &_estimated_transform);

  return true;
}

bool StageEstimateTransformBetweenPoints::isReady() const
{
  return this->input(IN_POINTS_A).numOfConnections() > 0
         &&
         this->input(IN_POINTS_B).numOfConnections() > 0;
}



bool StageConvertLaserScanToPoints::doProcess(processing::NoDataType&)
{
  using francor::base::LogDebug;
  using francor::base::LogError;
  const auto sensor_data = this->input(IN_SCAN).data<std::shared_ptr<base::SensorData>>();
  const auto& scan       = *std::static_pointer_cast<base::LaserScan>(sensor_data);
  const auto& ego_pose   = this->input(IN_EGO_POSE).numOfConnections() > 0 ? this->input(IN_EGO_POSE).data<base::Pose2d>() : base::Pose2d();

  // @todo replace debug messages with proper one
  LogDebug() << "uses scan pose " << scan.pose();
  LogDebug() << "uses ego pose " << ego_pose;

  if (!base::algorithm::point::convertLaserScanToPoints(scan, ego_pose, _resulted_points)) {
    LogError() << this->name() << ": error occurred during convertion. Can't convert laser scan.";
    return false;
  }

  LogDebug() << this->name() << ": converted " << _resulted_points.size() << " laser beams to points.";
  return true;
}

bool StageConvertLaserScanToPoints::doInitialization() 
{
  return true;
}

bool StageConvertLaserScanToPoints::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SCAN, "laser scan");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego pose");

  this->initializeOutputPort(OUT_POINTS, "points 2d", &_resulted_points);

  return true;
}

bool StageConvertLaserScanToPoints::validateInputData() const
{
  using francor::base::LogError;
  auto input_sensor_data = this->input(IN_SCAN).data<std::shared_ptr<base::SensorData>>();

  if (nullptr == input_sensor_data) {
    LogError() << this->name() << ": input sensor data pointer is null";
    return false;
  }
  if (nullptr == std::dynamic_pointer_cast<base::LaserScan>(input_sensor_data)) {
    LogError() << this->name() << ": input sensor data is not of type laser scan";
    return false;
  }

  return true;
}

bool StageConvertLaserScanToPoints::isReady() const
{
  return this->input(IN_SCAN).numOfConnections() > 0;
}



bool StageEstimateNormalsFromOrderedPoints::doProcess(processing::NoDataType&)
{
  using francor::base::LogDebug;
  using francor::base::LogError;

  const auto& points = this->input(IN_POINTS).data<base::Point2dVector>();

  if (auto result = estimateNormalsFromOrderedPoints(points, 5)) {
    _resulted_normals = std::move(*result);
  }
  else {
    LogError() << "Normal estimation wasn't successfull.";
    return false;
  }

  return true;
}

bool StageEstimateNormalsFromOrderedPoints::doInitialization()
{
  return true;
}

bool StageEstimateNormalsFromOrderedPoints::initializePorts()
{
  this->initializeInputPort<base::Point2dVector>(IN_POINTS, "points 2d");

  this->initializeOutputPort<std::vector<base::NormalizedAngle>>(OUT_NORMALS, "normals 2d", &_resulted_normals);

  return true;
}

bool StageEstimateNormalsFromOrderedPoints::isReady() const
{
  return this->input(IN_POINTS).numOfConnections() > 0;
}




bool StageExtractSensorPose::doProcess(processing::NoDataType&)
{
  using francor::base::LogDebug;

  const auto sensor_data = this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>();
  _sensor_pose = sensor_data->pose();
  LogDebug() << this->name() << ": extracted " << _sensor_pose << " from sensor data of sensor "
             << sensor_data->sensorName();

  return true;
}

bool StageExtractSensorPose::doInitialization()
{
  return true;
}

bool StageExtractSensorPose::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SENSOR_DATA, "sensor_data");

  this->initializeOutputPort<base::Pose2d>(OUT_SENSOR_POSE, "extracted sensor pose", &_sensor_pose);

  return true;
}

bool StageExtractSensorPose::validateInputData() const
{
  using francor::base::LogError;
  auto input_sensor_data = this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>();

  if (nullptr == input_sensor_data) {
    LogError() << this->name() << ": input sensor data pointer is null";
    return false;
  }

  return true;
}

bool StageExtractSensorPose::isReady() const
{
  return this->input(IN_SENSOR_DATA).numOfConnections() > 0;
}


} // end namespace algorithm

} // end namespace francor