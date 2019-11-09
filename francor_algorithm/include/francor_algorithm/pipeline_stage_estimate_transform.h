/**
 * Processing pipeline stages for estimation of transformation between two data sets.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 9. November 2019
 */
#pragma once

#include <francor_processing/data_processing_pipeline_stage.h>

#include <francor_base/transform.h>
#include <francor_base/point.h>

#include "francor_algorithm/icp.h"
#include "francor_algorithm/flann_point_pair_estimator.h"
#include "francor_algorithm/estimate_transform.h"

namespace francor {

namespace algorithm {

class StageEstimateTransformBetweenPoints final : public processing::ProcessingStage<processing::NoDataType>
{
public:
  enum Inputs {
    IN_POINTS_A = 0,
    IN_POINTS_B,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_TRANSFORM = 0,
    COUNT_OUTPUTS
  };

  struct Parameter
  {
    Parameter() { }

    std::size_t max_iterations = 100;
    double max_rms = 10.0;
    double termination_rms = 0.05;
  };

  StageEstimateTransformBetweenPoints(const Parameter& parameter = Parameter())
    : processing::ProcessingStage<processing::NoDataType>("estimate transform between points", COUNT_INPUTS, COUNT_OUTPUTS),
      _parameter(parameter),
      _icp(std::move(std::make_unique<FlannPointPairEstimator>()), estimateTransform)
  { }

private:
  bool doProcess(processing::NoDataType&) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;

  Parameter _parameter;
  Icp _icp;
  base::Transform2d _estimated_transform;
};



class StageConvertLaserScanToPoints final : public processing::ProcessingStage<processing::NoDataType>
{
public:
  enum Inputs {
    IN_SCAN = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    COUNT_OUTPUTS
  };

  StageConvertLaserScanToPoints()
    : processing::ProcessingStage<processing::NoDataType>("convert laser scan to points", COUNT_INPUTS, COUNT_OUTPUTS)
  { }

private:
  bool doProcess(processing::NoDataType&) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;

  base::Point2dVector _resulted_points;
};

} // end namespace algorithm

} // end namespace francor