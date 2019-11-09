/**
 * Processing pipeline stages for ego object.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include "francor_mapping/ego_object.h"

#include <francor_processing/data_processing_pipeline_stage.h>

namespace francor {

namespace mapping {

/**
 * \brief Estimating of laser scanner pose. Note: actually it is to simply for an extra stage, but the
 *        pipeline concept requires such a extra calculation step.
 */
// \todo invent a base class for all sensor data classes so this stage can be more generic usable
class StageEstimateLaserScannerPose final : public processing::ProcessingStage<EgoObject>
{
public:
  enum Inputs {
    IN_SCAN = 0,
    IN_SENSOR_POSE,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POSE,
    OUT_EGO_POSE,
    COUNT_OUTPUTS
  };

  StageEstimateLaserScannerPose()
    : processing::ProcessingStage<EgoObject>("estimate laser scanner pose", COUNT_INPUTS, COUNT_OUTPUTS)
  { }  

private:
  bool doProcess(EgoObject& ego) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;

  base::Pose2d _estimated_pose; //> estimated pose by this stage
  base::Pose2d _ego_pose;       //> pose of ego object
};


/**
 * \brief Updates Ego using connected inputs.
 */
class StageUpdateEgo final : public processing::ProcessingStage<EgoObject>
{
public:
  enum Inputs {
    IN_SENSOR_POSE = 0,
    IN_TRANSFORM,
    COUNT_INPUTS
  };
  enum Outputs {
    COUNT_OUTPUTS = 0
  };

  StageUpdateEgo()
    : processing::ProcessingStage<EgoObject>("update ego object", COUNT_INPUTS, COUNT_OUTPUTS)
  { }

private:
  bool doProcess(EgoObject& ego) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;
};


} // end namespace mapping

} // end namespace francor