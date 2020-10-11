/**
 * This file defines a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */

#pragma once

#include "francor_mapping/kalman_filter_model.h"

namespace francor {

namespace mapping {

/**
 * \brief This class is a kalman filter. A model is required. This class hold the state and covariances.
 */

// prototype declaration
// template <class ModelType, int Dummy>
// class KalmanFilter;

// specialization for KalmanFitlerModel
template <class ModelType>
class KalmanFilter
{
  using FilterModel = ModelType;

public:
  template <KinematicAttribute... SensorAttributes,
            std::size_t Dimension = KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>
  void process(const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
               const base::Matrix<typename FilterModel::data_type, Dimension, Dimension>& meas_covariance)
  {

  }               
  void predict(const double delta_t);
  template <KinematicAttribute... SensorAttributes,
            std::size_t Dimension = KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>
  void update(const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
              const base::Matrix<typename FilterModel::data_type, Dimension, Dimension>& meas_covariance)
  {

  }              

private:

  typename FilterModel::StateVector _state;
  typename FilterModel::Matrix _corvariances;
};

} // end namespace mapping

} // end namespace francor