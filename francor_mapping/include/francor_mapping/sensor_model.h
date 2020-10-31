/**
 * This file defines the sensor model class designed to work with the Kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 12. October 2020
 */

#pragma once

#include <francor_base/log.h>

#include "francor_mapping/kinematic_state_vector.h"
#include "francor_mapping/observation_matrix_builder.h"

namespace francor {

namespace mapping {

template <typename SensorData, class StateVector>
class SensorModel;

template <typename SensorData, KinematicAttribute... Attributes>
class SensorModel<SensorData, KinematicStateVector<KinematicAttributePack<Attributes...>>>
{
public:
  using StateVector = KinematicStateVector<KinematicAttributePack<Attributes...>>;
  using CovarianceMatrix = base::Matrix<typename StateVector::type, StateVector::size, StateVector::size>;

  template <KinematicAttribute... AttributesB>
  inline static const typename ObservationMatrix<KinematicAttributePack<Attributes...>,
                                                 KinematicAttributePack<AttributesB...>>::matrix_type&
  getObservationMatrix(const KinematicStateVector<KinematicAttributePack<AttributesB...>>&)
  {
    using ObservationType = ObservationMatrix<KinematicAttributePack<Attributes...>,
                                              KinematicAttributePack<AttributesB...>>;
    static const typename ObservationType::matrix_type observation_matrix = ObservationType::matrix();

    return observation_matrix;
  }

  static StateVector transformSensorData(const SensorData& sensor_data);
  // {
  //   static_assert(false, "Template method transformSensorData() has to be specialized.");
  // }

  static CovarianceMatrix transformCovariances(const SensorData& sensor_data);
  // {
  //   static_assert(false, "Template method transformCovariances() has to be specialized.");
  // }
};

} // end namespace mapping

} // end namespace francor