/**
 * This file defines the sensor data base class.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include "francor_base/pose.h"

namespace francor {

namespace base {

class SensorData
{
protected:
  SensorData(char const* const sensor_name, const double time_stamp, const Pose2d& sensor_pose = {})
    : _sensor_name(sensor_name), _time_stamp(time_stamp), _pose(sensor_pose) { }
  SensorData() = delete;

public:
  virtual ~SensorData() = default;

  inline char const* const sensorName() const { return _sensor_name; }
  inline double timeStamp() const { return _time_stamp; }
  inline const Pose2d& pose() const noexcept { return _pose; }

private:
  char const* _sensor_name;
  double _time_stamp;
  Pose2d _pose;
};

} // end namespace base

} // end namespace francor