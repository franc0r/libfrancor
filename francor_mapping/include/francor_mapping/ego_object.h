/**
 * Represents a ego object like the robot.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include <francor_base/pose.h>

#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"

namespace francor {

namespace mapping {

class EgoObject
{
public:
  using StateModel = KalmanFilter<EgoKalmanFilterModel>;

  EgoObject(const base::Pose2d& pose = base::Pose2d())
    : _state_model(0.0)
  {
    setPose(pose);
  }
  EgoObject(const StateModel::StateVector& initial_state,
            const StateModel::Matrix& initial_covariances,
            const double time_stamp = 0.0)
    : _state_model(0.0)            
  {
    _state_model.initialize(initial_state, initial_covariances, time_stamp);
  }

  inline base::Pose2d pose() const
  {
    return { {_state_model.state().x(), _state_model.state().y()}, _state_model.state().yaw() };
  }
  inline void setPose(const base::Pose2d& value)
  {
    StateModel::StateVector state;
    StateModel::Matrix covariances = StateModel::Matrix::Identity() * 0.1;

    state.x() = value.position().x();
    state.y() = value.position().y();

    state.yaw() = value.orientation();

    _state_model.initialize(state, covariances, _state_model.timeStamp());
  }

  inline const StateModel::Matrix& covariances() const { return _state_model.covariances(); }
  inline const StateModel::StateVector& stateVector() const { return _state_model.state(); }
  inline double timeStamp() { return _state_model.timeStamp(); }
  inline StateModel& model() { return _state_model; }

private:
  StateModel _state_model;
};

} // end namespace mapping

} // end namespace francor