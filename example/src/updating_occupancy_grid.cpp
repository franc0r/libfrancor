#include <francor_base/log.h>
#include <francor_base/transform.h>

#include <francor_vision/image.h>
#include <francor_vision/io.h>

#include <francor_mapping/pipeline_occupancy_grid.h>
#include <francor_mapping/algorithm/occupancy_grid.h>
#include <francor_mapping/pipeline_stage_ego_object.h>

#include <thread>
#include <random>

using francor::mapping::PipeSimulateLaserScan;
using francor::mapping::PipeUpdateOccupancyGrid;
using francor::mapping::PipeConvertLaserScanToPoints;
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
using francor::base::Angle;
using francor::base::NormalizedAngle;
using francor::base::LaserScan;

OccupancyGrid _grid_source;
OccupancyGrid _grid;
EgoObject _ego;
const Pose2d _sensor_pose({ 0.0, 0.0 }, Angle::createFromDegree(90.0));

PipeSimulateLaserScan _pipe_simulator;
PipeConvertLaserScanToPoints _pipe_convert_scan;
PipeUpdateOccupancyGrid _pipe_update_grid;

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
  const auto index_start_x = _grid.getIndexX(scan.pose().position().x() + _ego.pose().position().x());
  const auto index_start_y = _grid.getIndexY(scan.pose().position().y() + _ego.pose().position().y());
  Angle current_phi = scan.phiMin();
  const int radius_px = static_cast<int>(20.0 / 0.05);
  const Point2d position(scan.pose().position() + _ego.pose().position());

  drawLaserBeamOnImage(position, scan.pose().orientation()+ _ego.pose().orientation() + scan.phiMin(), 20,
                       cv::Scalar(0, 0, 240), image);
  drawLaserBeamOnImage(position, scan.pose().orientation()+ _ego.pose().orientation() + scan.phiMax(), 20,
                       cv::Scalar(0, 0, 240), image);
  cv::circle(image.cvMat(), {index_start_x, index_start_y }, radius_px, cv::Scalar(0, 0, 240), 2);

  for (const auto& distance : scan.distances())
  {
    drawLaserBeamOnImage(position, scan.pose().orientation() + _ego.pose().orientation() + current_phi,
                         (std::isnan(distance) ? scan.range() : distance), cv::Scalar(0, 240, 0), image);

    current_phi += scan.phiStep();
  }
}

bool loadGridFromFile(const std::string& file_name, OccupancyGrid& grid)
{
  const Image image(francor::vision::loadImageFromFile(file_name, francor::vision::ColourSpace::GRAY));

  return francor::mapping::algorithm::occupancy::createGridFromImage(image, 0.05, _grid_source);
}

void applyGaussianNoise(LaserScan& scan)
{
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 0.1);
  std::vector<double> modified_distances;

  modified_distances.reserve(scan.distances().size());

  for (const auto distance : scan.distances()) {
    modified_distances.push_back(distance + distribution(generator));
  }

  scan = LaserScan(modified_distances, scan.pose(), scan.phiMin(),
                   scan.phiMax(), scan.phiStep(), scan.range(), Angle::createFromDegree(1.0));
}

bool initialize(const std::string& file_name)
{
  if (!loadGridFromFile(file_name, _grid_source)) {
    LogError() << "Can't load occupancy grid from file.";
    return false;
  }

  if (!_grid.init(_grid_source.getNumCellsX(), _grid_source.getNumCellsY(), _grid_source.getCellSize())) {
    LogError() << "Can't initialize target grid.";
    return false;
  }

  _ego.setPose({ { 25.0, 0.0 }, 0.0 });

  if (!_pipe_simulator.initialize()) {
    LogError() << "Can't initialize \"" << _pipe_simulator.name() << "\" pipeline.";
    return false;
  }

  if (!_pipe_update_grid.initialize()) {
    LogError() << "Can't initialize \"" << _pipe_update_grid.name() << "\" pipeline.";
    return false;
  }

  return true;
}

bool processStep(const Vector2d& delta_position)
{
  const Transform2d transform({ Angle::createFromDegree(-2.33) }, delta_position);

  _ego.setPose(transform * _ego.pose());

  _pipe_simulator.input(PipeSimulateLaserScan::IN_SENSOR_POSE).assign(&_sensor_pose);
  auto start = std::chrono::system_clock::now();

  if (!_pipe_simulator.process(_grid_source, _ego)) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_simulator.name() << "\".";
    return false;
  }

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "reconstruct laser scan processing time = " << elapsed.count() << " us";


  LaserScan scan(_pipe_simulator.output(PipeSimulateLaserScan::OUT_SCAN).data<LaserScan>());
  applyGaussianNoise(scan);
  _pipe_convert_scan.input(PipeConvertLaserScanToPoints::IN_SCAN).assign(&scan);

  if (!_pipe_convert_scan.process()) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_convert_scan.name() << "\".";
    return false;
  }

  const auto& normals(_pipe_convert_scan.output(PipeConvertLaserScanToPoints::OUT_NORMALS).data<std::vector<NormalizedAngle>>());
  _pipe_update_grid.input(PipeUpdateOccupancyGrid::IN_SCAN).assign(&scan);
  _pipe_update_grid.input(PipeUpdateOccupancyGrid::IN_NORMALS).assign(&normals);
  start = std::chrono::system_clock::now();

  if (!_pipe_update_grid.process(_grid, _ego)) {
    LogError() << "Error occurred during processing of pipeline \"" << _pipe_update_grid.name() << "\".";
    return false;
  }

  end = std::chrono::system_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "updating occupancy grid processing time = " << elapsed.count() << " us";

  Image out_grid, out_grid_source;
  francor::mapping::algorithm::occupancy::convertGridToImage(_grid, out_grid);
  francor::mapping::algorithm::occupancy::convertGridToImage(_grid_source, out_grid_source);
  out_grid.transformTo(ColourSpace::BGR);
  out_grid_source.transformTo(ColourSpace::BGR);
  // drawLaserScanOnImage(scan, out_grid);

  cv::imshow("occupancy grid", out_grid.cvMat());
  cv::imshow("occupancy grid source", out_grid_source.cvMat());
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

  for (std::size_t step = 0; step < 800; ++step)
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