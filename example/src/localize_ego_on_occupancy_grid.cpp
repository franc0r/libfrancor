#include <francor_base/log.h>
#include <francor_base/transform.h>
#include <francor_base/algorithm/line.h>
#include <francor_base/algorithm/point.h>

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

void drawLaserBeamOnImage(const Point2d& start_point, const Angle phi, const double length,
                          const cv::Scalar& colour, Image& image)
{
  const Transform2d transform({ phi },
                              { start_point.x(), start_point.y() });
  const Point2d end(transform * Point2d(length, 0.0));

  const auto index_start_x = _grid.getIndexX(start_point.x());
  const auto index_start_y = _grid.getIndexY(start_point.y());
  const auto index_end_x = _grid.getIndexX(end.x());
  const auto index_end_y = _grid.getIndexY(end.y());

  cv::line(image.cvMat(), { index_start_x, index_start_y }, { index_end_x, index_end_y }, colour, 3);
}

void drawLaserScanOnImage(const LaserScan& scan, Image& image)
{
  const Transform2d tranform({ _ego_ground_truth.pose().orientation() },
                             { _ego_ground_truth.pose().position().x(), _ego_ground_truth.pose().position().y() });
  const Pose2d pose(tranform * scan.pose());
  const auto index_start_x = _grid.getIndexX(pose.position().x());
  const auto index_start_y = _grid.getIndexY(pose.position().y());
  Angle current_phi = scan.phiMin();
  const int radius_px = static_cast<int>(20.0 / 0.05);

  drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMin(), 20,
                       cv::Scalar(0, 0, 240), image);
  drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMax(), 20,
                       cv::Scalar(0, 0, 240), image);
  cv::circle(image.cvMat(), {index_start_x, index_start_y }, radius_px, cv::Scalar(0, 0, 240), 2);

  for (const auto& distance : scan.distances())
  {
    if (!(std::isnan(distance) || std::isinf(distance))) {
      drawLaserBeamOnImage(pose.position(), pose.orientation() + current_phi, distance,
                          cv::Scalar(0, 240, 0), image);
    }
                             
    current_phi += scan.phiStep();
  }
}

void drawPointsOnImage(const Point2dVector& points, Image& image)
{
  for (const auto& point : points)
  {
    const auto index_x = _grid.getIndexX(point.x());
    const auto index_y = _grid.getIndexY(point.y());

    cv::circle(image.cvMat(), { index_x, index_y }, 7, cv::Scalar(0, 0, 255), 3);
  }
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

  _ego.setPose({ { 25.0, 2.0 }, 0.0 });
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
  const Transform2d transform({ Angle::createFromDegree(-0.1) }, delta_position);

  _ego_ground_truth.setPose(transform * _ego_ground_truth.pose());
  std::cout << "ego ground truth " << _ego_ground_truth.pose() << std::endl;
  _pipe_simulator.input(PipeSimulateLaserScan::IN_SENSOR_POSE).assign(&_sensor_pose);
  auto start = std::chrono::system_clock::now();

  // generate new laser scan
  if (!_pipe_simulator.process(_grid, _ego_ground_truth)) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_simulator.name() << "\".";
    return false;
  }

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "reconstruct laser scan processing time = " << elapsed.count() << " us";

  // estimate pose using generated laser scan
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
  drawLaserScanOnImage(scan, out_grid);
  Point2dVector points;
  francor::base::algorithm::point::convertLaserScanToPoints(scan, _ego_ground_truth.pose(), points);
  drawPointsOnImage(points, out_grid);
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
    const Vector2d step_position(0.0, 0.02);

    if (!processStep(step_position)) {
      LogError() << "terminate application";
      return 3;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}