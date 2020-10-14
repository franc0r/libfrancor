/**
 * This file defines the sensor model class designed to work with the Kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include "francor_mapping/sensor_model.h"

#include "francor_base/pose_sensor_data.h"

namespace francor {

namespace mapping {

using PoseSensorModel = SensorModel<base::PoseSensorData,
                                    KinematicStateVector<KinematicAttributePack<KinematicAttribute::POS_X,
                                                                                KinematicAttribute::POS_Y,
                                                                                KinematicAttribute::YAW>>>;
                                                                              
template<>
PoseSensorModel::StateVector PoseSensorModel::transformSensorData(const base::PoseSensorData& sensor_data) const
{
  return { };
}


} // end namespace mapping

} // end namespace francor