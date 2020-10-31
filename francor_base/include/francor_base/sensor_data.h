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

private:
  char const* _sensor_name;
  double _time_stamp;
};

} // end namespace base

} // end namespace francor