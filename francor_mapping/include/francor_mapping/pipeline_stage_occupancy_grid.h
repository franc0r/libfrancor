/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include "francor_mapping/occupancy_grid.h"

#include <francor_base/point.h>
#include <francor_base/angle.h>

#include <francor_processing/data_processing_pipeline_stage.h>

namespace francor {

namespace mapping {

class StageReconstructPointsFromOccupancyGrid final : public processing::ProcessingStage<OccupancyGrid>
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

  struct Parameter 
  {
    Parameter() { }

    base::Angle phi_min         = 0.0;
    base::Angle phi_step        = 0.0;
    std::size_t num_laser_beams = 0;
    double      max_range       = 0.0;
  };

  StageReconstructPointsFromOccupancyGrid(const Parameter& parameter = Parameter())
    : processing::ProcessingStage<OccupancyGrid>("reconstruct points from occupancy grid", COUNT_INPUTS, COUNT_OUTPUTS),
      _parameter(parameter)
  { }

private:
  bool doProcess(OccupancyGrid& grid) final;
  bool doInitialization() final;
  bool initializePorts() final;
  bool isReady() const final;

  const Parameter _parameter;
  base::Point2dVector _reconstructed_points;
};

} // end namespace mapping

} // end namespace francor