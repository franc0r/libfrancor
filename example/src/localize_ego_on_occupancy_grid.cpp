#include <francor_base/log.h>
#include <francor_base/transform.h>
#include <francor_base/algorithm/line.h>

#include <francor_vision/image.h>
#include <francor_vision/io.h>

#include <francor_mapping/pipeline_occupancy_grid.h>
#include <francor_mapping/algorihm/occupancy_grid.h>

#include <thread>

using francor::mapping::PipeSimulateLaserScan;
using francor::mapping::PipeLocalizeAndUpdateEgo;
using francor::mapping::OccupancyGrid;
using francor::mapping::EgoObject;
using francor::vision::Image;
using francor::vision::ColourSpace;
using francor::base::Pose2d;
using francor::base::Point2d;
using francor::base::Point2dVector;
using francor::base::Transform2d;
using francor::base::LogError;
using francor::base::LogInfo;
using francor::base::LogDebug;
using francor::base::Vector2d;
using francor::base::Vector2i;
using francor::base::Angle;
using francor::base::LaserScan;

OccupancyGrid _grid;
EgoObject _ego;
EgoObject _ego_ground_truth;
const Pose2d _sensor_pose({ 0.0, 0.0 }, Angle::createFromDegree(90.0));

PipeSimulateLaserScan _pipe_simulator;
PipeLocalizeAndUpdateEgo _pipe_update_ego;

void drawPose(const Pose2d& pose, Image& image)
{
  const auto index_start_x = _grid.getIndexX(pose.position().x());
  const auto index_start_y = _grid.getIndexY(pose.position().y());

  const auto v_x = francor::base::algorithm::line::calculateV(pose.orientation());
  const auto v_y = francor::base::algorithm::line::calculateV(pose.orientation() + M_PI_2);

  const Vector2i end_x = (v_x * 40.0).cast<int>() + Vector2i(index_start_x, index_start_y);
  const Vector2i end_y = (v_y * 40.0).cast<int>() + Vector2i(index_start_x, index_start_y);

  cv::line(image.cvMat(), { index_start_x, index_start_y }, { end_x.x(), end_x.y() }, cv::Scalar(0, 0, 240), 3);
  cv::line(image.cvMat(), { index_start_x, index_start_y }, { end_y.x(), end_y.y() }, cv::Scalar(0, 240, 0), 3);
}

bool loadGridFromFile(const std::string& file_name, OccupancyGrid& grid)
{
  const Image image(francor::vision::loadImageFromFile(file_name, francor::vision::ColourSpace::GRAY));

  return francor::mapping::algorithm::occupancy::createGridFromImage(image, 0.05, grid);
}

bool initialize(const std::string& file_name)
{
  if (!loadGridFromFile(file_name, _grid)) {
    LogError() << "Can't load occupancy grid from file.";
    return false;
  }

  _ego.setPose({ { 25.0, 0.0 }, 0.0 });
  _ego_ground_truth.setPose(_ego.pose());

  if (!_pipe_simulator.initialize()) {
    LogError() << "Can't initialize \"" << _pipe_simulator.name() << "\" pipeline.";
    return false;
  }

  if (!_pipe_update_ego.initialize()) {
    LogError() << "Can't initialize \"" << _pipe_update_ego.name() << "\" pipeline.";
    return false;
  }

  return true;
}

bool processStep(const Vector2d& delta_position)
{
  const Transform2d transform({ Angle::createFromDegree(-2.0) }, delta_position);

  _ego_ground_truth.setPose(transform * _ego_ground_truth.pose());
  _pipe_simulator.input(PipeSimulateLaserScan::IN_SENSOR_POSE).assign(&_sensor_pose);
  auto start = std::chrono::system_clock::now();

  if (!_pipe_simulator.process(_grid, _ego_ground_truth)) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_simulator.name() << "\".";
    return false;
  }

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "reconstruct laser scan processing time = " << elapsed.count() << " us";

  LaserScan scan(_pipe_simulator.output(PipeSimulateLaserScan::OUT_SCAN).data<LaserScan>());
  _pipe_update_ego.input(PipeLocalizeAndUpdateEgo::IN_SCAN).assign(&scan);
  start = std::chrono::system_clock::now();

  if (!_pipe_update_ego.process(_ego, _grid)) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_update_ego.name() << "\".";
    return false;
  }      

  end = std::chrono::system_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "updating ego processing time = " << elapsed.count() << " us";  

  Image out_grid;
  francor::mapping::algorithm::occupancy::convertGridToImage(_grid, out_grid);
  out_grid.transformTo(ColourSpace::BGR);
  drawPose(_ego.pose(), out_grid);
  cv::imshow("occupancy grid", out_grid.cvMat());
  cv::waitKey(10);

  return true;
}

int main(int argc, char** argv)
{
  francor::base::setLogLevel(francor::base::LogLevel::DEBUG);

  if (argc < 2) {
    LogError() << "Too few arguments. Will exit...";
    LogInfo() << "Usage: " << argv[0] << " <path to grid gray scale image>";
    return 1;
  }

  if (!initialize(argv[1])) {
    LogError() << "Error occurred during initialization. Will exit...";
    return 2;
  }

  for (std::size_t step = 0; step < 500; ++step)
  {
    const Vector2d step_position(0.0, 0.05);

    if (!processStep(step_position)) {
      LogError() << "terminate application";
      return 3;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}