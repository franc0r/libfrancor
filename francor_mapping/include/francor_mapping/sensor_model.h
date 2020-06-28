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

#include <francor_base/log.h>
#include <francor_base/sensor_measurment.h>
#include <francor_base/exception.h>

#include "francor_mapping/occupancy_grid.h"

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

  OccupancyGrid createMeasurmentGrid(std::shared_ptr<const base::SensorMeasurment> sensor_measurement,
                                     const OccupancyGrid& status_grid) const
  {
    using LogDebug = base::Log<base::LogLevel::DEBUG, base::LogGroup::ALGORITHM, class_name>;
    using LogError = base::Log<base::LogLevel::ERROR, base::LogGroup::ALGORITHM, class_name>;

    LogDebug() << "check if sensor model can process grid type.";

    if (this->isCompatibleWithMeasurement(sensor_measurement)) {
      LogDebug() << "sensor type " << this->type() << " is compatible.";
      return std::move(this->createOccupancyGrid(sensor_measurement, status_grid));
    }
    else {
      LogError() << "sensor type " << this->type() << " is not compatible.";
      throw base::DataProcessingError("SensorModel::createMeasurementGrid(): sensor type and measurement type are"
                                      " incompatible.");
    }
  }                                     

protected:
  virtual OccupancyGrid createOccupancyGrid(std::shared_ptr<const base::SensorMeasurment> sensor_measurement,
                                            const OccupancyGrid& status_grid) const
  {
    using Log = base::Log<base::LogLevel::FATAL, base::LogGroup::COMPONENT, class_name>;
    Log() << "createOccupancyGrid() is not implemented but sensor type is compatible in general.";
    
    throw base::DataProcessingError("SensorModel::createOccupancyGrid() is not implemented but measurement type"
                                    " is compatible in general.");
  }

private:
  bool isCompatibleWithMeasurement(std::shared_ptr<const base::SensorMeasurment> sensor_measurement) const
  {
    if (std::strstr(this->type(), sensor_measurement->sensorType()) != nullptr) {
      return true;
    }
    else {
      return false;
    }
  }

  const char* _sensor_type;
  const std::string _sensor_name;

  static constexpr const char class_name[] = "SensorModel";
};

} // end namespace mapping

} // end namespace francor