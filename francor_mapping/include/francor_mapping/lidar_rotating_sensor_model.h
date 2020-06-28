/**
 * Represents a 2d lidar based rotating mirror/optic technology/concept. Note: this model is experimental!
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include "francor_mapping/sensor_model.h"

namespace francor {

namespace mapping {

namespace sensor {

namespace model {

class LidarRotating : public SensorModel
{
public:
  LidarRotating() = delete;
  LidarRotating(const std::string& sensor_name) : SensorModel("lidar_rotating", sensor_name) { }
  ~LidarRotating() override = default;

private:
  OccupancyGrid createOccupancyGrid(std::shared_ptr<const base::SensorMeasurment> sensor_measurement,
                                    const OccupancyGrid& status_grid) const override;

public:
  static constexpr const char class_name[] = "LidarRotation";
};

} // end namespace model

} // end namespace sensor

} // end namespace mapping

} // end namespace francor