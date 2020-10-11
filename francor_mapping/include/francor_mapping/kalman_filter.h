/**
 * This file defines a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */

#pragma once

#include <francor_base/log.h>

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
  bool process(const double time_stamp,
               const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
               const base::Matrix<typename FilterModel::data_type, Dimension, Dimension>& measurement_covariances,
               const base::Matrix<typename FilterModel::data_type, FilterModel::dimension, Dimension>& observation_matrix)
  {
    typename FilterModel::StateVector predicted_state;
    typename FilterModel::Matrix predicted_covariances;

    // predict internal states and covariances to given time stamp
    if (false == this->predictToTime(time_stamp, predicted_state, predicted_covariances)) {
      base::LogWarn() << "KalmanFilter::predict(): cancel prediction and keep previous state.";
      return false;
    }

    // update internal states and covariances using given measurements and predicted states
    this->update(time_stamp, measurements, measurement_covariances, predicted_state, predicted_covariances, observation_matrix);
    
    return true;
  }               
  
  bool predictToTime(const double time_stamp,
                     typename FilterModel::StateVector& predicted_state,
                     typename FilterModel::Matrix& predicted_covariances) const
  {
    if (time_stamp < _time_stamp) {
      base::LogError() << "KalmanFilter::predictToTime(): requested prediction of given time stamp failed."
                       << " Given time stamp is in the past."
                       << " model time stamp = " << _time_stamp << ", given time stamp = " << time_stamp;
      return false;                       
    }

    constexpr FilterModel model;
    const double delta_time = this->calculateDeltaTime(time_stamp);
    const auto prediction_matrix = model.getPredictionMatrix(delta_time);

    predicted_state       = prediction_matrix * _state;
    predicted_covariances = prediction_matrix * _corvariances * prediction_matrix.transpose();

    return true;
  }

private:
  template <KinematicAttribute... SensorAttributes,
            std::size_t Dimension = KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>
  void update(const double time_stamp,
              const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
              const base::Matrix<typename FilterModel::data_type, Dimension, Dimension>& measurement_covariances,
              const typename FilterModel::StateVector& predicted_state,
              const typename FilterModel::Matrix& predicted_covariances,
              const base::Matrix<typename FilterModel::data_type, FilterModel::dimension, Dimension>& observation_matrix)
  {
    using SensorMatrix = base::Matrix<typename FilterModel::data_type, Dimension, Dimension>;
    const auto identity_matrix = base::Matrix<typename FilterModel::data_type, FilterModel::dimension, Dimension>::Identity();

    // reduce predicted covariances to sensor dimension space using observation matrix
    const SensorMatrix predicted_covariances_sensor_space = observation_matrix * predicted_covariances * observation_matrix.transpose();

    // calculate the innovation and its covariances
    const typename FilterModel::StateVector::Vector innovation = measurements - predicted_state;
    const SensorMatrix innovation_covariances = predicted_covariances_sensor_space + measurement_covariances;

    // calculate kalman gain matrix and update the state and state covariances
    const typename FilterModel::Matrix kalman_gain = predicted_covariances * observation_matrix.transpose() * innovation_covariances.inverse();

    _state = predicted_state + kalman_gain * innovation;
    _corvariances = (identity_matrix - kalman_gain * observation_matrix) * predicted_covariances;

    // set new time stamp
    _time_stamp = time_stamp;
  }              

  double calculateDeltaTime(const double future_timestamp) const
  {
    if (future_timestamp <= _time_stamp) {
      return 0.0;
    }
    else {
      return future_timestamp - _time_stamp;
    } 
  }

  double _time_stamp{0};
  typename FilterModel::StateVector _state;
  typename FilterModel::Matrix _corvariances;
};

} // end namespace mapping

} // end namespace francor