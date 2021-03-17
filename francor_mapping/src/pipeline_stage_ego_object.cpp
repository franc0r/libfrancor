/**
 * Processing pipeline stages for ego object.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_stage_ego_object.h"
#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"
#include "francor_mapping/ego_motion_sensor_model.h"
#include "francor_mapping/pose_sensor_model.h"

#include <francor_base/laser_scan.h>
#include <francor_base/transform.h>

namespace francor {

namespace mapping {

bool StagePredictEgo::doProcess(EgoObject& ego)
{
  using francor::base::LogDebug;
  using francor::base::LogError;
  
  const auto time_stamp = this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>()->timeStamp();

  // predict to given time stamp without updating ego object
  if (ego.timeStamp() < time_stamp) {
    LogDebug() << this->name() << ": predict ego object state to time = " << time_stamp;
    EgoObject::StateModel::StateVector predicted_state;
    EgoObject::StateModel::Matrix predicted_covariance;

    if (false == ego.model().predictToTime(time_stamp, predicted_state, predicted_covariance)) {
      LogError() << this->name() << ": time prediction of ego object failed.";
      return false;
    }

    EgoObject predicted_ego_object(predicted_state, predicted_covariance, time_stamp);
    _ego_pose = predicted_ego_object.pose();
  }
  // target time is in the past
  else {
    LogDebug() << this->name() << ": target time = " << time_stamp << ", is in the past. Skip prediction.";
    _ego_pose = ego.pose();
  }

  return true;
}

bool StagePredictEgo::doInitialization()
{
  return true;
}

bool StagePredictEgo::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SENSOR_DATA, "sensor_data");

  this->initializeOutputPort(OUT_EGO_POSE, "predicted_ego pose", &_ego_pose);

  return true;
}

bool StagePredictEgo::validateInputData() const
{
  using francor::base::LogError;

  if (nullptr == this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>()) {
    LogError() << this->name() << ": input sensor data pointer is null";
    return false;
  }

  return true;
}

bool StagePredictEgo::isReady() const
{
  return this->input(IN_SENSOR_DATA).numOfConnections() > 0;
}



bool StageUpdateEgo::doProcess(EgoObject& ego)
{
  using francor::base::LogDebug;
  using francor::base::LogError;

  const auto sensor_data = this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>();
  LogDebug() << this->name() << ": updating ego object with measurement to time stamp " << sensor_data->timeStamp();

  if (nullptr != std::dynamic_pointer_cast<base::PoseSensorData>(sensor_data)) {
    LogDebug() << this->name() << ": received sensor data of type pose sensor data";
    const auto pose_measurment = std::static_pointer_cast<base::PoseSensorData>(sensor_data);

    const auto observation_matrix = PoseSensorModel::getObservationMatrix(EgoObject::StateModel::StateVector{});
    const auto pose_state = PoseSensorModel::transformSensorData(*pose_measurment);
    const auto pose_covariances = PoseSensorModel::transformCovariances(*pose_measurment);

    if (false == ego.model().process(pose_measurment->timeStamp(), pose_state, pose_covariances, observation_matrix)) {
      LogError() << this->name() << ": ego object update failed";
    }
  }
  else {
    LogError() << this->name() << ": sensor data type can't be handled, cancel ego object update";
    return false;
  }

  return true;
}

bool StageUpdateEgo::doInitialization()
{
  return true;
}

bool StageUpdateEgo::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SENSOR_DATA, "sensor data");

  return true;
}

bool StageUpdateEgo::validateInputData() const
{
  using francor::base::LogError;

  if (nullptr == this->input(IN_SENSOR_DATA).data<std::shared_ptr<base::SensorData>>()) {
    LogError() << this->name() << ": input sensor data pointer is null";
    return false;
  }

  return true;
}

bool StageUpdateEgo::isReady() const
{
  return this->input(IN_SENSOR_DATA).numOfConnections() > 0;
}

} // end namespace mapping

} // end namespace francor