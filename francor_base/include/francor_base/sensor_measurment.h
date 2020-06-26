/**
 * Base class for all measurments of a sensor.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include "francor_base/pose.h"

namespace francor {

namespace base {

class SensorMeasurment
{
public:
  SensorMeasurment() = delete;
  virtual ~SensorMeasurment() = default;

  struct SensorName {
    static constexpr const char* NONE = "none";
  };

  struct SensorType {
    static constexpr const char* NONE              = "no_type";
    static constexpr const char* LIDAR_2D          = "lidar_2d";
    static constexpr const char* LIDAR_2D_ROTATION = "lidar_2d_rotation";
  };

protected:
  SensorMeasurment(const std::string& sensor_name, const char* sensor_type, const Pose2d& sensor_pose)
    : _sensor_name(sensor_name),
      _sensor_type(sensor_type),
      _sensor_pose(sensor_pose)
  { }

public:
  inline const Pose2d& sensorPose() const noexcept { return _sensor_pose; } 
  inline const std::string& sensorName() const noexcept { return _sensor_name; }
  inline const char* sensorType() const noexcept { return _sensor_type; } 

private:
  std::string _sensor_name;
  const char* _sensor_type;
  Pose2d _sensor_pose;
};

} // end namespace base

} // end namespace francor