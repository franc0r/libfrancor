/**
 * Processing pipeline for occupancy grid
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/pipeline_occupancy_grid.h"

#include <francor_base/pose.h>
#include <francor_base/pose_sensor_data.h>

namespace francor {

namespace mapping {

bool PipeSimulateLaserScan::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(StageReconstructLaserScanFromOccupancyGrid::IN_SENSOR_POSE)
                             .connect(this->input(IN_SENSOR_POSE));
  ret &= std::get<0>(_stages).input(StageReconstructLaserScanFromOccupancyGrid::IN_EGO_POSE)
                             .connect(this->input(IN_EGO_POSE));                             
  ret &= std::get<0>(_stages).output(StageReconstructLaserScanFromOccupancyGrid::OUT_SCAN)
                             .connect(this->output(OUT_SCAN));
  ret &= std::get<0>(_stages).input(StageReconstructLaserScanFromOccupancyGrid::IN_TIME_STAMP)
                             .connect(this->input(IN_TIME_STAMP));                                                          

  ret &= std::get<1>(_stages).input(StageReconstructPointsFromOccupancyGrid::IN_SENSOR_POSE)
                             .connect(this->input(IN_SENSOR_POSE));                            
  ret &= std::get<1>(_stages).input(StageReconstructPointsFromOccupancyGrid::IN_EGO_POSE)
                             .connect(this->input(IN_EGO_POSE));                               
  ret &= std::get<1>(_stages).output(StageReconstructPointsFromOccupancyGrid::OUT_POINTS)
                             .connect(this->output(OUT_POINTS));

  return ret;                                                          
}

bool PipeSimulateLaserScan::initializePorts()
{
  this->initializeInputPort<base::Pose2d>(IN_SENSOR_POSE, "sensor pose");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego_pose");
  this->initializeInputPort<double>(IN_TIME_STAMP, "time_stamp");

  this->initializeOutputPort<base::Point2dVector>(OUT_POINTS, "points 2d");
  this->initializeOutputPort<base::LaserScan>(OUT_SCAN, "laser scan");

  return true;
}



bool PipeUpdateOccupancyGrid::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(StagePushLaserScanToOccupancyGrid::IN_EGO_POSE)
                             .connect(this->input(IN_EGO_POSE));
  ret &= std::get<0>(_stages).input(StagePushLaserScanToOccupancyGrid::IN_SCAN)
                             .connect(this->input(IN_SCAN));
  ret &= std::get<0>(_stages).input(StagePushLaserScanToOccupancyGrid::IN_NORMALS)
                             .connect(this->input(IN_NORMALS));                             

  return ret;                             
}

bool PipeUpdateOccupancyGrid::initializePorts()
{
  this->initializeInputPort<base::LaserScan>(IN_SCAN, "laser scan");
  this->initializeInputPort<std::vector<base::AnglePiToPi>>(IN_NORMALS, "normals");
  this->initializeInputPort<base::Pose2d>(IN_EGO_POSE, "ego_pose");

  return true;
}



bool PipeLocalizeOnOccupancyGrid::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(algorithm::StageExtractSensorPose::IN_SENSOR_DATA)
                             .connect(this->input(IN_SCAN));

  ret &= std::get<1>(_stages).input(StagePredictEgo::IN_SENSOR_DATA)
                             .connect(this->input(IN_SCAN));

  ret &= std::get<2>(_stages).input(algorithm::StageConvertLaserScanToPoints::IN_SCAN)
                             .connect(this->input(IN_SCAN));
  ret &= std::get<2>(_stages).input(algorithm::StageConvertLaserScanToPoints::IN_EGO_POSE)
                             .connect(std::get<1>(_stages).output(StagePredictEgo::OUT_EGO_POSE));                            

  ret &= std::get<3>(_stages).input(StageReconstructPointsFromOccupancyGrid::IN_SENSOR_POSE)
                             .connect(std::get<0>(_stages).output(algorithm::StageExtractSensorPose::OUT_SENSOR_POSE)); 
  ret &= std::get<3>(_stages).input(StageReconstructPointsFromOccupancyGrid::IN_EGO_POSE)
                             .connect(std::get<1>(_stages).output(StagePredictEgo::OUT_EGO_POSE));
  ret &= std::get<3>(_stages).output(StageReconstructPointsFromOccupancyGrid::OUT_POINTS)
                             .connect(this->output(OUT_POINTS));                                                          

  ret &= std::get<4>(_stages).input(algorithm::StageEstimateTransformBetweenPoints::IN_POINTS_A)
                             .connect(std::get<3>(_stages).output(StageReconstructPointsFromOccupancyGrid::OUT_POINTS));
  ret &= std::get<4>(_stages).input(algorithm::StageEstimateTransformBetweenPoints::IN_POINTS_B)
                             .connect(std::get<2>(_stages).output(algorithm::StageConvertLaserScanToPoints::OUT_POINTS));

  ret &= std::get<5>(_stages).input(StageCreatePoseMeasurement::IN_DELTA_POSE)
                             .connect(std::get<4>(_stages).output(algorithm::StageEstimateTransformBetweenPoints::OUT_TRANSFORM));
  ret &= std::get<5>(_stages).input(StageCreatePoseMeasurement::IN_EGO_POSE)
                             .connect(std::get<1>(_stages).output(StagePredictEgo::OUT_EGO_POSE));
  ret &= std::get<5>(_stages).input(StageCreatePoseMeasurement::IN_SENSOR_DATA)
                             .connect(this->input(IN_SCAN));
  ret &= std::get<5>(_stages).output(StageCreatePoseMeasurement::OUT_SENSOR_DATA)
                             .connect(this->output(OUT_POSE_MEASUREMENT));

  return ret;
}

bool PipeLocalizeOnOccupancyGrid::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SCAN, "laser scan");

  this->initializeOutputPort<base::Point2dVector>(OUT_POINTS, "reconstructed points 2d");
  this->initializeOutputPort<std::shared_ptr<base::PoseSensorData>>(OUT_POSE_MEASUREMENT, "result_localiztation");

  return true;
}



bool PipeConvertLaserScanToPoints::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(algorithm::StageConvertLaserScanToPoints::IN_SCAN)
                             .connect(this->input(IN_SCAN));
  ret &= std::get<0>(_stages).output(algorithm::StageConvertLaserScanToPoints::OUT_POINTS)
                             .connect(this->output(OUT_POINTS));                             

  ret &= std::get<1>(_stages).input(algorithm::StageEstimateNormalsFromOrderedPoints::IN_POINTS)
                             .connect(std::get<0>(_stages).output(algorithm::StageConvertLaserScanToPoints::OUT_POINTS));
  ret &= std::get<1>(_stages).output(algorithm::StageEstimateNormalsFromOrderedPoints::OUT_NORMALS)
                             .connect(this->output(OUT_NORMALS));

  return ret;                             
}

bool PipeConvertLaserScanToPoints::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SCAN, "laser scan");

  this->initializeOutputPort<base::Point2dVector>(OUT_POINTS, "points 2d");
  this->initializeOutputPort<std::vector<base::AnglePiToPi>>(OUT_NORMALS, "normals");

  return true;
}



bool PipeUpdateEgoObject::configureStages()
{
  bool ret = true;

  ret &= std::get<0>(_stages).input(StageUpdateEgo::IN_SENSOR_DATA)
                             .connect(this->input(IN_SENSOR_DATA));

  return ret;                             
}

bool PipeUpdateEgoObject::initializePorts()
{
  this->initializeInputPort<std::shared_ptr<base::SensorData>>(IN_SENSOR_DATA, "sensor_data");

  return true;
}

} // end namespace mapping

} // end namespace francor