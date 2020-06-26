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
  // delete default constructor
  SensorMeasurment() = delete;
  // virtual destructor
  virtual ~SensorMeasurment() = default;

  // to provide default values
  // \todo think about the correct location for SensorName and SensorType.
  struct SensorName {
    static constexpr const char* NONE = "none";
  };

  struct SensorType {
    static constexpr const char* NONE              = "no_type";           //> in case to type is available
    static constexpr const char* LIDAR_2D          = "lidar_2d";          //> represents all types of 2d lidars
    static constexpr const char* LIDAR_2D_ROTATION = "lidar_2d_rotation"; //> represents lidars with rotating measurement unit or mirror
  };

protected:
  /**
   * \brief Constructs with all necessary attributes for a sensor measurment.
   * 
   * \param sensor_name Name of the sensor that measured this data.
   * \param sensor_type The type of the sensor that measured this data.
   * \param sensor_pose The pose of the sensor that measured this data. (in ego/vehicle) frame)
   */
  SensorMeasurment(const std::string& sensor_name, const char* sensor_type, const Pose2d& sensor_pose)
    : _sensor_name(sensor_name),
      _sensor_type(sensor_type),
      _sensor_pose(sensor_pose)
  { }

public:
  /**
   * \brief Returns the pose of the sensor that measured this data.
   * 
   * \return 2d sensor pose.
   */
  inline const Pose2d& sensorPose() const noexcept { return _sensor_pose; } 
  /**
   * \brief Returns the name of the sensor that measured this data.
   * 
   * \return Sensor name.
   */
  inline const std::string& sensorName() const noexcept { return _sensor_name; }
  /**
   * \brief Returns the type of the sensor that measured this data.
   * 
   * \return Sensor type.
   */
  inline const char* sensorType() const noexcept { return _sensor_type; } 

private:
  std::string _sensor_name;
  const char* _sensor_type;
  Pose2d _sensor_pose;
};

} // end namespace base

} // end namespace francor