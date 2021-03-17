/**
 * Processing pipeline stages for calculation new ego pose from delta and last pose.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 31.10.2020
 */
#pragma once

#include "francor_mapping/ego_object.h"

#include <francor_base/pose_sensor_data.h>

#include <francor_processing/data_processing_pipeline_stage.h>

namespace francor {

namespace mapping {

/**
 * \brief Estimating of laser scanner pose. Note: actually it is to simply for an extra stage, but the
 *        pipeline concept requires such a extra calculation step.
 */
// \todo invent a base class for all sensor data classes so this stage can be more generic usable
class StageCreatePoseMeasurement final : public processing::ProcessingStage<processing::NoDataType>
{
public:
  enum Inputs {
    IN_DELTA_POSE = 0, // delta pose as transformation
    IN_EGO_POSE,       // ego pose
    IN_SENSOR_DATA,    // any sensor data used to get the current time stamp
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_SENSOR_DATA = 0, // created pose measurement
    COUNT_OUTPUTS
  };

  StageCreatePoseMeasurement()
    : processing::ProcessingStage<processing::NoDataType>("estimate laser scanner pose", COUNT_INPUTS, COUNT_OUTPUTS)
  { }  

private:
  bool doProcess(processing::NoDataType&) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;
  bool validateInputData() const final;

  std::shared_ptr<base::PoseSensorData> _sensor_data;
};

} // end namespace mapping

} // end namespace francor