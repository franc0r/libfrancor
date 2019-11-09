/**
 * Processing pipeline stages for estimation of transformation between two data sets.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 9. November 2019
 */

#include "francor_algorithm/pipeline_stage_estimate_transform.h"

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

  if (!_icp.estimateTransform(point_set_a, point_set_b, _estimated_transform)) {
    LogError() << this->name() << ": error occurred during estimation. Can't estimate transformatin.";
    return false;
  }

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

  const auto& scan = this->input(IN_SCAN).data<base::LaserScan>();

  if (!base::algorithm::point::convertLaserScanToPoints(scan, _resulted_points)) {
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
  this->initializeInputPort<base::LaserScan>(IN_SCAN, "laser scan");

  this->initializeOutputPort(OUT_POINTS, "points 2d", &_resulted_points);

  return true;
}

bool StageConvertLaserScanToPoints::isReady() const
{
  return this->input(IN_SCAN).numOfConnections() > 0;
}


} // end namespace algorithm

} // end namespace francor