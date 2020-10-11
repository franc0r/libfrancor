/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */

#pragma once

#include "francor_mapping/kinematic_state_vector.h"

#include <francor_base/matrix.h>

namespace francor {

namespace mapping {

/**
 * \brief This class is the base class for kalman filter models. A model defines the state vector and
 *        provides functionality to predict this state.
 */

// prototype declaration
template <class StateVectorType>
class KalmanFilterModel;

// specialization for KinematicStateVector
template <KinematicAttribute... Attributes>
class KalmanFilterModel<KinematicAttributePack<Attributes...>>
{
public:
  using StateVector = KinematicStateVector<KinematicAttributePack<Attributes...>>;
  static constexpr std::size_t dimension = StateVector::size;
  using data_type = typename StateVector::type;
  using Matrix = francor::base::Matrix<data_type, dimension, dimension>;

  constexpr KalmanFilterModel() { }

  /**
   * \brief Provides a matrix that can be used to predict in time the current state.
   * \param delta_time Is delta t. The state wil be predicted by this time step.
   * \return Prediction matrix with dimension of this model.
   */
  virtual Matrix getPredictionMatrix(const StateVector& current_state, const double delta_time) const = 0;

  /**
   * \brief Creates the system noise covariance matrix based on delta t.
   * \param delta_time Is delta t. The created noise covariance matrix will reflect this time period.
   * \return System noise covariance matrix.
   */
  virtual Matrix getSystemNoiseMatrix(const double delta_time) const = 0;
};

} // end namespace mapping

} // end namespace francor