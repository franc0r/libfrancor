#include "francor_mapping/ego_kalman_filter_model.h"

namespace francor {

namespace mapping {

EgoKalmanFilterModel::Matrix EgoKalmanFilterModel::getPredictionMatrix(const StateVector& current_state, 
                                                                       const double delta_time) const
{
  enum Index : Eigen::Index {
    POS_X    = StateVector::getAttributeIndex<KinematicAttribute::POS_X   >(),
    POS_Y    = StateVector::getAttributeIndex<KinematicAttribute::POS_Y   >(),
    VEL      = StateVector::getAttributeIndex<KinematicAttribute::VEL     >(),
    VEL_X    = StateVector::getAttributeIndex<KinematicAttribute::VEL_X   >(),
    VEL_Y    = StateVector::getAttributeIndex<KinematicAttribute::VEL_Y   >(),
    ACC      = StateVector::getAttributeIndex<KinematicAttribute::ACC     >(),
    YAW      = StateVector::getAttributeIndex<KinematicAttribute::YAW     >(),
    YAW_RATE = StateVector::getAttributeIndex<KinematicAttribute::YAW_RATE>(),
  };

  Matrix prediction_matrix = Matrix::Identity();

  const data_type distance = delta_time * current_state.velocity() + 0.5 * delta_time * delta_time * current_state.acceleration();

  // prediction_matrix(POS_X, YAW) = distance * std::cos(current_state.yaw());
  prediction_matrix(POS_X, VEL) = delta_time * std::cos(current_state.yaw());
  // prediction_matrix(POS_X, VEL) = delta_time * std::sin(current_state.yaw());
  prediction_matrix(POS_X, ACC) = 0.5 * delta_time * delta_time * std::cos(current_state.yaw());

  // prediction_matrix(POS_Y, YAW) = distance * std::sin(current_state.yaw());
  prediction_matrix(POS_Y, VEL) = delta_time * std::sin(current_state.yaw());
  // prediction_matrix(POS_Y, VEL) = delta_time * std::cos(current_state.yaw());
  prediction_matrix(POS_Y, ACC) = 0.5 * delta_time * delta_time * std::sin(current_state.yaw());

  prediction_matrix(VEL, ACC) = delta_time * 0.5;
  prediction_matrix(VEL_X, ACC) = delta_time * std::cos(current_state.yaw());
  // prediction_matrix(VEL_X, VEL) = delta_time * std::cos(current_state.yaw());
  prediction_matrix(VEL_Y, ACC) = delta_time * std::sin(current_state.yaw());
  // prediction_matrix(VEL_Y, VEL) = delta_time * std::sin(current_state.yaw());

  prediction_matrix(YAW, YAW_RATE) = delta_time;

  return prediction_matrix;
}

EgoKalmanFilterModel::Matrix EgoKalmanFilterModel::getSystemNoiseMatrix(const double delta_time) const
{
  StateVector noise_variances;

  noise_variances.x() = 10.0 * delta_time;
  noise_variances.y() =  0.2 * delta_time;

  noise_variances.velocity()  = 10.0 * delta_time;
  noise_variances.velocityX() = 10.0 * delta_time;
  noise_variances.velocityY() = 10.0 * delta_time;

  noise_variances.acceleration() = 10.0 * delta_time * delta_time;

  noise_variances.yaw()     = base::Angle::createFromDegree(90) * delta_time;
  noise_variances.yawRate() = base::Angle::createFromDegree(90) * delta_time * delta_time;

  const StateVector::Vector variance_vector = noise_variances;

  return variance_vector * variance_vector.transpose();
}                                                                        

} // end namespace mapping

} // end namespace francor