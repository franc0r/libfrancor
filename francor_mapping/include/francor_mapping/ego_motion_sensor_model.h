/**
 * This file defines the ego motion sensor model class designed to work with the Kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include "francor_mapping/sensor_model.h"

#include "francor_base/ego_motion_sensor_data.h"

namespace francor {

namespace mapping {

using EgoMotionSensorModel = SensorModel<base::EgoMotionSensorData,
                                         KinematicStateVector<KinematicAttributePack<KinematicAttribute::VEL,
                                                                                     KinematicAttribute::YAW_RATE>>;
                                                                              
template<>
EgoMotionSensorModel::StateVector
EgoMotionSensorModel::transformSensorData(const base::EgoMotionSensorData& sensor_data) const
{
  StateVector measurement;

  measurement.velocity() = sensor_data.velocity();
  measurement.yawRate()  = sensor_data.rawRate();

  return measurement;
}

template<>
EgoMotionSensorModel::CovarianceMatrix
EgoMotionSensorModel::transformCovariances(const base::EgoMotionSensorData& sensor_data) const
{
  return sensor_data.covariances();
}


} // end namespace mapping

} // end namespace francor