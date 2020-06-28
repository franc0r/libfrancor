#include "francor_mapping/lidar_rotating_sensor_model.h"

namespace francor {

namespace mapping {

namespace sensor {

namespace model {

using LogDebug = base::Log<base::LogLevel::DEBUG, base::LogGroup::ALGORITHM, LidarRotating::class_name>;
using LogError = base::Log<base::LogLevel::ERROR, base::LogGroup::ALGORITHM, LidarRotating::class_name>;

OccupancyGrid LidarRotating::createOccupancyGrid(std::shared_ptr<const base::SensorMeasurment> sensor_measurement,
                                                 const OccupancyGrid& status_grid) const
{
  OccupancyGrid grid;

  grid.init()
}                                                

} // end namespace model

} // end namespace sensor

} // end namespace mapping

} // end namespace francor