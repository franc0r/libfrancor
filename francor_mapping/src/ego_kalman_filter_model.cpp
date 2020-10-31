#include "francor_mapping/ego_kalman_filter_model.h"

#include <francor_base/angle.h>

#include <iostream>
namespace francor {

namespace mapping {

EgoKalmanFilterModel::Matrix EgoKalmanFilterModel::getPredictionMatrix(const StateVector& current_state, 
                                                                       const double delta_time) const
{
  enum Index : Eigen::Index {
    POS_X    = StateVector::getAttributeIndex<KinematicAttribute::POS_X   >(),
    POS_Y    = StateVector::getAttributeIndex<KinematicAttribute::POS_Y   >(),
    VEL      = StateVector::getAttributeIndex<KinematicAttribute::VEL     >(),
    ACC      = StateVector::getAttributeIndex<KinematicAttribute::ACC     >(),
    YAW      = StateVector::getAttributeIndex<KinematicAttribute::YAW     >(),
    YAW_RATE = StateVector::getAttributeIndex<KinematicAttribute::YAW_RATE>(),
  };

  Matrix prediction_matrix = Matrix::Identity();

  prediction_matrix(POS_X, VEL) = delta_time * std::cos(current_state.yaw());
  prediction_matrix(POS_X, ACC) = 0.5 * delta_time * delta_time * std::cos(current_state.yaw());

  prediction_matrix(POS_Y, VEL) = delta_time * std::sin(current_state.yaw());
  prediction_matrix(POS_Y, ACC) = 0.5 * delta_time * delta_time * std::sin(current_state.yaw());

  prediction_matrix(VEL, ACC) = delta_time;

  prediction_matrix(YAW, YAW_RATE) = delta_time;

  return prediction_matrix;
}

EgoKalmanFilterModel::Matrix EgoKalmanFilterModel::getSystemNoiseMatrix(const StateVector& current_state,
                                                                        const double delta_time) const
{
  Matrix noise_matrix = Matrix::Zero();

  // add jerk system noise
  {
    constexpr double jerk_variance = 1.0 * 1.0;
    StateVector noise_variances;

    noise_variances.x() = (delta_time * delta_time * delta_time / 6.0) * std::cos(current_state.yaw());
    noise_variances.y() = (delta_time * delta_time * delta_time / 6.0) * std::sin(current_state.yaw());
    noise_variances.velocity() = 0.5 * delta_time * delta_time;
    noise_variances.acceleration() = delta_time;

    const StateVector::Vector noise_vector(static_cast<StateVector::Vector>(noise_variances));
    noise_matrix += jerk_variance * noise_vector * noise_vector.transpose();
  }
  // add yaw acceleration noise
  {
    constexpr double yaw_acceleration_variance = base::Angle::createFromDegree(45) * base::Angle::createFromDegree(45);
    StateVector noise_variances;

    noise_variances.yaw() = 0.5 * delta_time * delta_time;
    noise_variances.yawRate() = delta_time;

    const StateVector::Vector noise_vector(static_cast<StateVector::Vector>(noise_variances));
    noise_matrix += yaw_acceleration_variance * noise_vector * noise_vector.transpose();

  }

  return noise_matrix;
}                                                                        

} // end namespace mapping

} // end namespace francor