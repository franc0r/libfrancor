/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_processing/data_processing_pipeline.h>

#include "francor_mapping/pipeline_stage_occupancy_grid.h"

namespace francor {

namespace mapping {

using PipeSimulateLaserScanParent = processing::ProcessingPipeline<OccupancyGrid,                           // model type
                                                                   StageReconstructPointsFromOccupancyGrid  // reconstruct points stage
                                                                   >;

class PipeSimulateLaserScan final : public PipeSimulateLaserScanParent
{
public:
  enum Inputs {
    IN_SENSOR_POSE = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    COUNT_OUTPUTS
  };

  PipeSimulateLaserScan() : PipeSimulateLaserScanParent("simulate laser scan", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final;
  bool initializePorts() final;
};

} // end namespace mapping

} // end namespace francor