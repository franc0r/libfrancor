/**
 * Represents a ego object like the robot.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include <francor_base/pose.h>

#include "francor_mapping/ego_kalman_filter_model.h"

namespace francor {

namespace mapping {

class EgoObject
{
public:
  EgoObject(const base::Pose2d& pose = base::Pose2d())
  {
    _state.x() = pose.position().x();
    _state.y() = pose.position().y();

    _state.yaw() = pose.orientation();
  }

  inline constexpr base::Pose2d pose() const noexcept { return {{_state.x(), _state.y()}, _state.yaw()}; }
  inline constexpr void setPose(const base::Pose2d& value)
  {
    _state.x() = value.position().x();
    _state.y() = value.position().y();

    _state.yaw() = value.orientation();
  }

  inline EgoKalmanFilterModel::Matrix& covariances() { return _covariances; }
  inline const EgoKalmanFilterModel::Matrix& covariances() const { return _covariances; }

  inline EgoKalmanFilterModel::StateVector& stateVector() { return _state; }
  inline double& timeStamp() { return _time_stamp; }

private:
  double _time_stamp{0};
  EgoKalmanFilterModel::StateVector _state;
  EgoKalmanFilterModel::Matrix _covariances;
};

} // end namespace mapping

} // end namespace francor