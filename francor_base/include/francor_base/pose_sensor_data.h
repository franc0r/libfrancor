/**
 * This file defines the pose sensor data class.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include "francor_base/sensor_data.h"
#include "francor_base/pose.h"
#include "francor_base/matrix.h"

namespace francor {

namespace base {

class PoseSensorData : public SensorData
{
public:
  PoseSensorData(const double time_stamp, const Pose2d& pose,
                 const Matrix3d& covariances, char const* const sensor_name = "unkown")
    : SensorData(sensor_name, time_stamp),
      _pose(pose),
      _covariances(covariances)
  { }

  const Pose2d& pose() const { return _pose; }
  const Matrix3d& covariances() const { return _covariances; }

private:
  const Pose2d _pose;
  const Matrix3d _covariances;
};

} // end namespace base

} // end namespace francor