/**
 * Processing pipeline for tsd slam
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include "francor_mapping/tsd_grid.h"
#include "francor_mapping/algorihm/tsd.h"
#include "francor_mapping/ego_object.h"

#include <francor_base/laser_scan.h>
#include <francor_base/point.h>
#include <francor_base/algorithm/point.h>
#include <francor_base/algorithm/transform.h>

#include <francor_algorithm/icp.h>
#include <francor_algorithm/flann_point_pair_estimator.h>
#include <francor_algorithm/estimate_transform.h>

#include <francor_processing/data_processing_pipeline.h>

namespace francor {

namespace mapping {

class PushLaserScanToTsdGrid final : public processing::ProcessingStage<TsdGrid>
{
public:
  enum Inputs {
    IN_LASER_SCAN = 0,
    IN_EGO_POSE,
    COUNT_INPUTS
  };
  enum Outpus {
    COUNT_OUTPUTS = 0
  };

  PushLaserScanToTsdGrid()
    : processing::ProcessingStage<TsdGrid>("push laser scan to tsd grid", COUNT_INPUTS, COUNT_OUTPUTS)
  { }

private:
  bool doProcess(TsdGrid& grid) final
  {
    // get laser scan and create for each measurement (distance) an ray
    const auto& laser_scan = this->input(IN_LASER_SCAN).data<base::LaserScan>();
    const auto& ego_pose   = this->input(IN_EGO_POSE  ).data<base::Pose2d>();

    algorithm::tsd::pushLaserScanToGrid(grid, laser_scan, ego_pose);

    return true;
  }
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::LaserScan>(IN_LASER_SCAN, "laser scan");
    this->initializeInputPort<base::Pose2d>   (IN_EGO_POSE  , "ego pose"  );

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_LASER_SCAN).numOfConnections() > 0
           &&
           this->input(IN_EGO_POSE).numOfConnections() > 0;
  }
};




/**
 * \brief Reconstruct points from tsd grid using a lidar sensor model.
 */
class ReconstructPointsFromTsdGrid final : public processing::ProcessingStage<TsdGrid>
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
    base::Angle phi_min         = 0.0;
    base::Angle phi_step        = 0.0;
    std::size_t num_laser_beams = 0;
    double      max_range       = 0.0;
  };

  ReconstructPointsFromTsdGrid(const Parameter& parameter)
    : processing::ProcessingStage<TsdGrid>("reconstruct points from tsd grid", COUNT_INPUTS, COUNT_OUTPUTS),
      _parameter(parameter)
  { }

private:
  bool doProcess(TsdGrid& grid) final
  {
    using francor::base::LogError;

    const auto& sensor_pose = this->input(IN_SENSOR_POSE).data<base::Pose2d>();

    if (!algorithm::tsd::reconstructPointsFromGrid(grid,
                                                   sensor_pose,
                                                   _parameter.phi_min,
                                                   _parameter.phi_step,
                                                   _parameter.num_beams,
                                                   _parameter.max_range,
                                                   _reconstructed_points))
    {
      LogError() << this->name() << ": reconstruct points from tsd grid failed.";
      return false;
    }              

    return true;                                    
  }
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor pose");

    this->initializeOutputPort(OUT_POINTS, "points 2d", &_reconstructed_points);

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_SENSOR_POSE).numOfConnections() > 0;
  }

  const Parameter _parameter;
  base::Point2dVector _reconstructed_points;
}



/**
 * \brief Estimate new pose base on new measurement (laser scan).
 */
class EstimatePose final : public processing::ProcessingStage<EgoObject>
{
public:
  enum Inputs {
    IN_SENSOR_POINTS = 0,
    IN_MAP_POINTS,
    COUNT_INPUTS
  };
  enum Outputs {
    COUNT_OUTPUTS = 0
  };

  EstimatePose()
   : processing::ProcessingStage<EgoObject>("estimate pose", COUNT_INPUTS, COUNT_OUTPUTS),
     _icp(std::make_unique<algorithm::FlannPointPairEstimator>(), algorithm::estimateTransform)
  { }

private:
  bool doProcess(EgoObject& ego) final
  {
    using francor::base::LogError;
    using francor::base::LogDebug;

    // get input data
    const auto& sensor_points = this->input(IN_SENSOR_POINTS).data<base::Point2dVector>();
    const auto& map_points    = this->input(IN_MAP_POINTS   ).data<base::Point2dVector>();
  
    // estimate transform using icp
    base::Transform2d transform;

    if (!_icp.estimateTransform(map_points, sensor_points, transform)) {
      LogError() << this->name() << ": error occurred during executing estimate transform.";
      return false;
    }

    // update ego pose with estimated transformation
    ego.setPose(transform * ego.pose());
    LogDebug() << this->name() << ": estimated transform " << transform;

    return true;
  }
  bool doInitialization() final
  {
    _icp.setMaxIterations(100);
    _icp.setMaxRms(10.0);
    _icp.setTerminationRms(0.05);

    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::Point2dVector>(IN_SENSOR_POINTS, "sensor points");
    this->initializeInputPort<base::point2dVector>(IN_MAP_POINTS   , "map points");

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_SENSOR_POINTS).numOfConnections() > 0
           &&
           this->input(IN_MAP_POINTS).numOfConnections() > 0;
  }

  algorithm::Icp _icp;
};




/**
 * \brief Converts a laser scan to 2d points.
 */
class LaserScanToPoints : public processing::ProcessingStage<NoDataType>
{
public:
  enum Inputs {
    IN_SCAN = 0,
    IN_EGO_POSE,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POINTS = 0,
    COUNT_OUTPUTS
  };

  LaserScanToPoints() : processing::ProcessingStage<NoDataType>("laser scan to points", COUNT_INPUTS, COUNT_OUTPUTS) { }

private:
  bool doProcess(NoDataType&) final
  {
    using francor::base::LogError;

    const auto& scan = this->input(IN_SCAN).data<base::LaserScan>();

    // converts laser scan to points in frame laser sensor
    if (!base::algorithm::point::convertLaserScanToPoints(scan, _converted_points)) {
      LogError() << this->name() << ": error occurred during converting of laser scan to points.";
      return false;
    }
    // if ego pose is provided then transform points to the ego frame
    if (this->input(IN_EGO_POSE).numConnections() > 0) {
      const auto ego_pose = this->input(IN_EGO_POSE).data<base::Pose2d>();
      base::algorithm::transform::transformPointVector(ego_pose, _converted_points);
    }

    return true;
  }
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::LaserScan>(IN_SCAN    , "laser scan");
    this->initializeInputPort<base::Pose2d>   (IN_EGO_POSE, "ego pose"  );

    this->initializeOutputPort(OUT_POINTS, "points 2d", &_converted_points);

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_SCAN).numOfConnections() > 0;
  }

  base::Point2dVector _converted_points;
};



/**
 * \brief Estimating of laser scanner pose. Note: actually it is to simply for an extra stage, but the
 *        pipeline concept requires such a extra calculation step.
 */
class EstimateLaserScannerPose : processing::ProcessingStage<EgoObject>
{
public:
  enum Inputs {
    IN_SCAN = 0,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_POSE,
    COUNT_OUTPUTS
  };

  EstimateLaserScannerPose()
    : public processing::ProcessingStage<EgoObject>("estimate laser scanner pose", COUNT_INPUTS, COUNT_OUTPUTS)
  { }  

private:
  bool doProcess(EgoObject& ego) final
  {
    using francor::base::LogDebug;
    
    const auto pose_laser(this->input(IN_SCAN).data<base::LaserScan>().pose());

    base::Transform2d t_laser_ego({ pose_laser.orientation() }, pose_laser.position());
    _estimated_pose = t_laser_ego * ego.pose();
    LogDebug() << this->name() << ": estimated " << _estimated_pose;

    return true;
  }
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::LaserScan>(IN_SCAN, "laser scan");

    this->initializeOutputPort(OUT_POSE, "pose", &_estimated_pose);

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_SCAN).numOfConnections() > 0;
  }

  base::Pose2d _estimated_pose;
};


/**
 * \brief Processing pipeline for localize ego on a tsd grid map.
 */
using PipeLocalizeOnTsdGridParent = processing::ProcessingPipeline<TsdGrid,                      // model type
                                                                   EstimateLaserScannerPose,     // estimate pose in frame grid stage
                                                                   LaserScanToPoints,            // laser scan to points stage
                                                                   ReconstructPointsFromTsdGrid, // reconstruct points stage
                                                                   EstimatePose                  // estimate pose stage
                                                                   >;

class PipeLocalizeOnTsdGrid : public PipeLocalizeOnTsdGridParent
{
public:
  PipeLocalizeOnTsdGrid("localize on tsd grid", 1, 0) { }

private:
  bool configureStages() final
  {
    bool ret = true;
    
    ret &= std::get<0>(_stages).input(EstimateLaserScannerPose::IN_SCAN).connect(this->input(0));

    ret &= std::get<1>(_stages).input(LaserScanToPoints::IN_SCAN).connect(this->input(0));
    ret &= std::get<1>(_stages).input(LaserScanToPoints::IN_EGO_POSE)
                               .connect(std::get<0>(_stages).output(EstimateLaserScannerPose::OUT_POSE));

    ret &= std::get<2>(_stages).input(ReconstructPointsFromTsdGrid::IN_SENSOR_POSE)
                               .connect(std::get<1>(_stages).output(EstimateLaserScannerPose::OUT_POSE));

    ret &= std::get<3>(_stages).input(EstimatePose::IN_SENSOR_POINTS)
                               .connect(std::get<1>(_stages).output(LaserScanToPoints::OUT_POINTS));
    ret &= std::get<3>(_stages).input(EstimatePose::IN_MAP_POINTS)
                               .connect(std::get<2>(_stages).output(ReconstructPointsFromTsdGrid::OUT_POINTS));

    return true;                                                              
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::LaserScan>(0, "laser scan");
    return true;
  }
};

} // end namespace mapping

} // end namespace francor