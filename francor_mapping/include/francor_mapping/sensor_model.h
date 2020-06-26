/**
 * Base class for sensor models that are used to handle grid operations like updating grid with measurements
 * and reconstructing from grid. The sensor models shall represents the caracteristics of an physical sensor.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include <string>
#include <cstring>
#include <memory>

#include "francor_base/log.h"
#include "francor_base/sensor_measurment.h"

namespace francor {

namespace mapping {

class OccupancyGrid;

class SensorModel
{
public:
  SensorModel() = delete;
  virtual ~SensorModel() = default;

protected:
  SensorModel(const char* sensor_type, const std::string& sensor_name)
    : _sensor_type(sensor_type),
      _sensor_name(sensor_name)
  { }

public:
  inline const std::string& name() const noexcept { return _sensor_name; }
  inline const char* type() const noexcept { return _sensor_type; }

  bool updateGrid(OccupancyGrid& grid, std::shared_ptr<const base::SensorMeasurment> sensor_measurement) const
  {
    using Log = base::Log<base::LogLevel::DEBUG, base::LogGroup::ALGORITHM, ClassName>;

    Log() << "check if sensor model can process grid type.";

    if (std::strstr(this->type(), base::SensorMeasurment::SensorType::LIDAR_2D) != nullptr) {
      Log() << "sensor type " << this->type() << " is compatible.";
      return this->updateGridImpl(grid, sensor_measurement);
    }
    else {
      Log() << "sensor type " << this->type() << " is not compatible.";
      return false;
    }
  }

protected:
  virtual bool updateGridImpl(OccupancyGrid& grid, std::shared_ptr<const base::SensorMeasurment> sensor_measurement) const
  {
    using Log = base::Log<base::LogLevel::FATAL, base::LogGroup::COMPONENT, ClassName>;
    Log() << "updateGridImpl() is not implemented but sensor type is compatible in general.";
    return false;
  }

private:
  const char* _sensor_type;
  const std::string _sensor_name;

  static constexpr const char ClassName[] = "SensorModel";
};

} // end namespace mapping

} // end namespace francor