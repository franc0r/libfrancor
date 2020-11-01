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

  KalmanFilter(const double time_stamp)
    : _time_stamp(time_stamp) { }

  template <KinematicAttribute... SensorAttributes>
  bool process(const double time_stamp,
               const KinematicStateVector<KinematicAttributePack<SensorAttributes...>>& measurements,
               const base::Matrix<typename FilterModel::data_type,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>& measurement_covariances,
               const base::Matrix<typename FilterModel::data_type,
                                           KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                           FilterModel::dimension>& observation_matrix)
  {
    StateVector predicted_state;
    Matrix predicted_covariances;

    // predict internal states and covariances to given time stamp
    if (false == this->predictToTime(time_stamp, predicted_state, predicted_covariances)) {
      base::LogWarn() << "KalmanFilter::process(): cancel prediction and keep previous state.";
      return false;
    }

    // update internal states and covariances using given measurements and predicted states
    this->update(time_stamp, measurements, measurement_covariances, predicted_state, predicted_covariances, observation_matrix);
    
    return true;
  }               

  bool predictToTime(const double time_stamp)
  {
    StateVector predicted_state;
    Matrix predicted_covariances;

    // predict internal states and covariances to given time stamp
    if (false == this->predictToTime(time_stamp, predicted_state, predicted_covariances)) {
      base::LogWarn() << "KalmanFilter::predictToTime(): cancel prediction and keep previous state.";
      return false;
    }

    // take predicted values as new state
    _state        = predicted_state;
    _corvariances = predicted_covariances;
    _time_stamp   = time_stamp;

    return true;
  } 

  bool predictToTime(const double time_stamp, StateVector& predicted_state, Matrix& predicted_covariances) const
  {
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
                            model.getSystemNoiseMatrix(_state, delta_time);

    return true;
  }

  void initialize(const StateVector& initial_state, const Matrix& initial_covariances, const double start_time = 0.0)
  {
    _state = initial_state;
    _corvariances = initial_covariances;
    _time_stamp = start_time;
  }

  inline const StateVector& state() const { return _state; }
  inline const Matrix& covariances() const { return _corvariances; }
  inline double timeStamp() const { return _time_stamp; }

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
                                 KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                 FilterModel::dimension>& observation_matrix)
  {
    using SensorMatrix = base::Matrix<typename FilterModel::data_type,
                                      KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size,
                                      KinematicStateVector<KinematicAttributePack<SensorAttributes...>>::size>;
    using SensorStateVector = KinematicStateVector<KinematicAttributePack<SensorAttributes...>>;                            
    using SensorVector = typename SensorStateVector::Vector;

    const auto identity_matrix = Matrix::Identity();

    // reduce predicted covariances to sensor dimension space using observation matrix
    const SensorMatrix predicted_covariances_sensor_space = observation_matrix * predicted_covariances * observation_matrix.transpose();
    const SensorStateVector predicted_state_sensor_space(observation_matrix * static_cast<typename StateVector::Vector>(predicted_state));

    // calculate the innovation and its covariances
    const SensorVector innovation = static_cast<SensorVector>(measurements - predicted_state_sensor_space);
    const SensorMatrix innovation_covariances = predicted_covariances_sensor_space + measurement_covariances;

    // calculate kalman gain matrix and update the state and state covariances
    const auto kalman_gain = predicted_covariances * observation_matrix.transpose() * innovation_covariances.inverse();
    _state = static_cast<typename StateVector::Vector>(predicted_state) + kalman_gain * innovation;
    _corvariances = (identity_matrix - kalman_gain * observation_matrix) * predicted_covariances;
    // _corvariances(0, 0) = std::max(0.01, _corvariances(0, 0));
    std::cout << "KALMAN: state:" << std::endl << static_cast<typename StateVector::Vector>(_state) << std::endl;
    std::cout << "KALMAN: covariances:" << std::endl << _corvariances << std::endl;
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

  double _time_stamp;
  StateVector _state;
  Matrix _corvariances;
};

} // end namespace mapping

} // end namespace francor