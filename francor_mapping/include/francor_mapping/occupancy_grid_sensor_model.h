/**
 * This file defines a observation matrix that uses functions for state transformations (matrix multiplication) 
 * for state vectors and covariances.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 1. November 2020
 */
#pragma once

#include <francor_base/sensor_data.h>

#include "francor_mapping/occupancy_grid.h"

namespace francor {

namespace mapping {

class OccupancyGridSensorModel
{
public:
  OccupancyGrid transformSensorData(const base::SensorData& sensor_data) const;
};

} // end namespace mapping

} // end namespace francor