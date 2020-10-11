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
public:

  using FilterModel = ModelType;
  using StateVector = typename FilterModel::StateVector;
  using Matrix = typename FilterModel::Matrix;


  template <KinematicAttribute... SensorAttributes>
  bool process(const double time_stamp,
               const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
               const base::Matrix<typename FilterModel::data_type,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>& measurement_covariances,
               const base::Matrix<typename FilterModel::data_type,
                                           FilterModel::dimension,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>& observation_matrix)
  {
    if (false == _is_initialized) {
      base::LogError() << "KalmanFilter isn't initialized. The filter must be intialized before it can be used."
                          " Cancel process() method.";
      return false;
    }

    StateVector predicted_state;
    Matrix predicted_covariances;

    // predict internal states and covariances to given time stamp
    if (false == this->predictToTime(time_stamp, predicted_state, predicted_covariances)) {
      base::LogWarn() << "KalmanFilter::predict(): cancel prediction and keep previous state.";
      return false;
    }

    // update internal states and covariances using given measurements and predicted states
    this->update(time_stamp, measurements, measurement_covariances, predicted_state, predicted_covariances, observation_matrix);
    
    return true;
  }               
  
  bool predictToTime(const double time_stamp, StateVector& predicted_state, Matrix& predicted_covariances) const
  {
    if (false == _is_initialized) {
      base::LogError() << "KalmanFilter isn't initialized. The filter must be intialized before it can be used."
                          " Cancel predictToTime() method.";
      return false;
    }
    if (time_stamp < _time_stamp) {
      base::LogError() << "KalmanFilter::predictToTime(): requested prediction of given time stamp failed."
                       << " Given time stamp is in the past."
                       << " model time stamp = " << _time_stamp << ", given time stamp = " << time_stamp;
      return false;                       
    }

    constexpr FilterModel model;
    const double delta_time = this->calculateDeltaTime(time_stamp);
    const auto prediction_matrix = model.getPredictionMatrix(_state, delta_time);
    // std::cout << "prediction matrix:" << std::endl << prediction_matrix << std::endl;
    predicted_state       = prediction_matrix * static_cast<typename StateVector::Vector>(_state);
    predicted_covariances = prediction_matrix * _corvariances * prediction_matrix.transpose() +
                            model.getSystemNoiseMatrix(delta_time);

    return true;
  }

  void initialize(const StateVector& initial_state, const Matrix& initial_covariances, const double start_time = 0.0)
  {
    _state = initial_state;
    _corvariances = initial_covariances;
    _time_stamp = start_time;
    _is_initialized = true;
  }

private:
  template <KinematicAttribute... SensorAttributes>
  void update(const double time_stamp,
              const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
              const base::Matrix<typename FilterModel::data_type,
                                 KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                 KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>& measurement_covariances,
              const StateVector& predicted_state,
              const Matrix& predicted_covariances,
              const base::Matrix<typename FilterModel::data_type,
                                 FilterModel::dimension,
                                 KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>& observation_matrix)
  {
    using SensorMatrix = base::Matrix<typename FilterModel::data_type,
                                      KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                      KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>;
    using TransformMatrix = base::Matrix<typename FilterModel::data_type,
                                         FilterModel::dimension,
                                         KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>;                                      
    using SensorVector = typename StateVector::Vector;

    const auto identity_matrix = TransformMatrix::Identity();

    // reduce predicted covariances to sensor dimension space using observation matrix
    const SensorMatrix predicted_covariances_sensor_space = observation_matrix * predicted_covariances * observation_matrix.transpose();

    // calculate the innovation and its covariances
    const SensorVector innovation = static_cast<SensorVector>(measurements) - static_cast<SensorVector>(predicted_state);
    const SensorMatrix innovation_covariances = predicted_covariances_sensor_space + measurement_covariances;

    // calculate kalman gain matrix and update the state and state covariances
    const Matrix kalman_gain = predicted_covariances * observation_matrix.transpose() * innovation_covariances.inverse();

    _state = static_cast<typename StateVector::Vector>(predicted_state) + kalman_gain * innovation;
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
  bool _is_initialized{false};
  StateVector _state;
  Matrix _corvariances;
};

} // end namespace mapping

} // end namespace francor