/**
 * This file defines the ego motion data class.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include "francor_base/sensor_data.h"
#include "francor_base/angle.h"
#include "francor_base/vector.h"
#include "francor_base/matrix.h"

namespace francor {

namespace base {

class EgoMotionSensorData : public SensorData
{
public:
  EgoMotionSensorData(const double time_stamp, const double velocity, const Angle yaw_rate,
                      const Matrix2d& covariances, char const* const sensor_name = "unkown")
    : SensorData(sensor_name, time_stamp),
      _velocity(velocity),
      _yaw_rate(yaw_rate),
      _covariances(covariances)
  { }

  inline const double velocity() const { return _velocity; }
  inline const double yawRate() const { return _yaw_rate; }
  inline const Matrix2d& covariances() const { return _covariances; }

private:
  const double _velocity;
  const Angle _yaw_rate;
  const Matrix2d _covariances;
};

} // end namespace base

} // end namespace francor