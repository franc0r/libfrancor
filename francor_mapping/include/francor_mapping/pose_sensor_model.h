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
PoseSensorModel::StateVector
PoseSensorModel::transformSensorData(const base::PoseSensorData& sensor_data) const
{
  StateVector measurement;

  measurement.x() = sensor_data.pose().position().x();
  measurement.y() = sensor_data.pose().position().y();

  measurement.yaw() = sensor_data.pose().orientation();

  return measurement;
}

template<>
PoseSensorModel::CovarianceMatrix
PoseSensorModel::transformCovariances(const base::PoseSensorData& sensor_data) const
{
  return sensor_data.covariances();
}


} // end namespace mapping

} // end namespace francor