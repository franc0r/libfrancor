/**
 * This file defines the sensor data base class.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

namespace francor {

namespace base {

class SensorData
{
protected:
  SensorData(char const* const sensor_name, const double time_stamp)
    : _sensor_name(sensor_name), _time_stamp(time_stamp) { }
  SensorData() = delete;

public:
  virtual ~SensorData() = default;

  char const* const sensorName() const { return _sensor_name; }
  double timeStamp() const { return _time_stamp; }

  SensorData& operator=(const SensorData& rhs) 
  {
    _sensor_name = rhs._sensor_name;
    _time_stamp = rhs._time_stamp;
    return *this;
  }

private:
  char const* const _sensor_name;
  const double _time_stamp;
};

} // end namespace base

} // end namespace francor