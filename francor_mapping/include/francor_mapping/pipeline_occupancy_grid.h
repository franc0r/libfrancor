/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_processing/data_processing_pipeline.h>

#include <francor_algorithm/pipeline_stage_estimate_transform.h>

#include "francor_mapping/pipeline_stage_occupancy_grid.h"
#include "francor_mapping/pipeline_stage_ego_object.h"
#include "francor_mapping/pipeline_stage_create_pose_measurement.h"

namespace francor {

namespace mapping {

using PipeSimulateLaserScanParent = processing::ProcessingPipeline<OccupancyGrid,                              // model type
                                                                   StageReconstructLaserScanFromOccupancyGrid, // reconstruct scan stage
                                                                   StageReconstructPointsFromOccupancyGrid     // reconstruct points stage
                                                                   >;

class PipeSimulateLaserScan final : public PipeSimulateLaserScanParent
{
public:
  enum Inputs {
    IN_SENSOR_POSE = 0,
    IN_EGO_POSE,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    OUT_SCAN,
    COUNT_OUTPUTS
  };

  PipeSimulateLaserScan() : PipeSimulateLaserScanParent("simulate laser scan", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final;
  bool initializePorts() final;
};



using PipeUpdateOccupancyGridParent = processing::ProcessingPipeline<OccupancyGrid,                    // model type
                                                                     StagePushLaserScanToOccupancyGrid // update grid stage                                                                     
                                                                     >;

class PipeUpdateOccupancyGrid final : public PipeUpdateOccupancyGridParent
{
public:
  enum Inputs {
    IN_SCAN = 0,
    IN_NORMALS,
    IN_EGO_POSE,
    COUNT_INPUTS
  };
  enum Outputs {
    COUNT_OUTPUTS = 0
  };

  PipeUpdateOccupancyGrid() : PipeUpdateOccupancyGridParent("update occupancy grid", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final;
  bool initializePorts() final;
};                                                                    



using PipeLocalizeOnOccupancyGridParent = processing::ProcessingPipeline<EgoObject,                                   // model type
                                                                      StagePredictEgo,                                // predicts ego to laser scan time stamp
                                                                      algorithm::StageConvertLaserScanToPoints,       // convert scan stage
                                                                      StageReconstructPointsFromOccupancyGrid,        // reconstruct points stage
                                                                      algorithm::StageEstimateTransformBetweenPoints, // estimate transform stage
                                                                      StageCreatePoseMeasurement                      // converts the result to a pose measurement 
                                                                      >;

class PipeLocalizeOnOccupancyGrid final : public PipeLocalizeOnOccupancyGridParent
{
public:
  enum Inputs {
    IN_SCAN = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    OUT_POSE_MEASUREMENT,
    COUNT_OUTPUTS
  };

  PipeLocalizeOnOccupancyGrid() : PipeLocalizeOnOccupancyGridParent("localize and update ego", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final; 
  bool initializePorts() final;
};


using PipeConvertLaserScanToPointsParent = processing::ProcessingPipeline<processing::NoDataType,
                                                                          algorithm::StageConvertLaserScanToPoints,         // convert scan stage
                                                                          algorithm::StageEstimateNormalsFromOrderedPoints  // estimate normals stage
                                                                          >;

class PipeConvertLaserScanToPoints final : public PipeConvertLaserScanToPointsParent
{
public:
  enum Inputs {
    IN_SCAN = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    OUT_NORMALS,
    COUNT_OUTPUTS
  };

  PipeConvertLaserScanToPoints() : PipeConvertLaserScanToPointsParent("convert laser scan to points", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final; 
  bool initializePorts() final;
};                                                                          


using PipeUpdateEgoObjectParent = processing::ProcessingPipeline<EgoObject,
                                                                 StageUpdateEgo   // updates ego state using sensor data
                                                                 >;

class PipeUpdateEgoObject final : public PipeUpdateEgoObjectParent
{
public:
  enum Inputs {
    IN_SENSOR_DATA = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    COUNT_OUTPUTS
  };

  PipeUpdateEgoObject() : PipeUpdateEgoObjectParent("update ego object", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool configureStages() final; 
  bool initializePorts() final;
};  

} // end namespace mapping

} // end namespace francor