#include <francor_base/log.h>
#include <francor_base/transform.h>

#include <francor_vision/image.h>
#include <francor_vision/io.h>

#include <francor_mapping/pipeline_occupancy_grid.h>
#include <francor_mapping/algorithm/occupancy_grid.h>
#include <francor_mapping/pipeline_stage_ego_object.h>

#include <thread>

using francor::mapping::PipeSimulateLaserScan;
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
using francor::base::LaserScan;

OccupancyGrid _grid;
EgoObject _ego;
const Pose2d _sensor_pose({ 0.0, 0.0 }, Angle::createFromDegree(90.0));

PipeSimulateLaserScan pipeline;

void drawPoseOnImage(const Pose2d& pose, Image& image)
{
  const auto index = _grid.find().cell().index(pose.position());
  constexpr std::size_t axis_length = 20;

  for (std::size_t i = 0; i < axis_length; ++i)
  {
    image(index.y(), index.x() + i).r() = 255;
    image(index.y(), index.x() + i).g() =   0;
    image(index.y(), index.x() + i).b() =   0;

    image(index.y() + i, index.x()).r() =   0;
    image(index.y() + i, index.x()).g() = 255;
    image(index.y() + i, index.x()).b() =   0;
  }
}

void drawPointsOnImage(const Point2dVector& points, Image& image)
{
  for (const auto& point : points)
  {
    const auto index = _grid.find().cell().index(point);
    cv::circle(image.cvMat(), { static_cast<int>(index.x()), static_cast<int>(index.y()) }, 7, cv::Scalar(0, 0, 255), 3);
  }
}

void drawLaserBeamOnImage(const Point2d& start_point, const Angle phi, const double length,
                          const cv::Scalar& colour, Image& image)
{
  const Transform2d transform({ phi },
                              { start_point.x(), start_point.y() });
  const Point2d end(transform * Point2d(length, 0.0));

  const auto index_start = _grid.find().cell().index(start_point);
  const auto index_end   = _grid.find().cell().index(end);

  cv::line(image.cvMat(),
           { static_cast<int>(index_start.x()), static_cast<int>(index_start.y()) },
           { static_cast<int>(index_end.x()), static_cast<int>(index_end.y()) },
           colour,
           3);
}

void drawLaserScanOnImage(const LaserScan& scan, Image& image)
{
  const Transform2d tranform({ _ego.pose().orientation() }, { _ego.pose().position().x(), _ego.pose().position().y() });
  const Pose2d pose(tranform * scan.pose());
  const auto index_start = _grid.find().cell().index(pose.position());
  Angle current_phi = scan.phiMin();
  const int radius_px = static_cast<int>(20.0 / 0.05);

  drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMin(), 20,
                       cv::Scalar(0, 0, 240), image);
  drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMax(), 20,
                       cv::Scalar(0, 0, 240), image);
  cv::circle(image.cvMat(), {static_cast<int>(index_start.x()), static_cast<int>(index_start.y()) }, radius_px, cv::Scalar(0, 0, 240), 2);

  for (const auto& distance : scan.distances())
  {
    if (!(std::isnan(distance) || std::isinf(distance))) {
      drawLaserBeamOnImage(pose.position(), pose.orientation() + current_phi, distance,
                          cv::Scalar(0, 240, 0), image);
    }
                             
    current_phi += scan.phiStep();
  }
}

bool loadGridFromFile(const std::string& file_name, OccupancyGrid& grid)
{
  const Image image(francor::vision::loadImageFromFile(file_name, francor::vision::ColourSpace::GRAY));

  return francor::mapping::algorithm::occupancy::createGridFromImage(image, 0.05, _grid);
}

bool initialize(const std::string& file_name)
{
  _ego.setPose({ { 25.0, 0.0 }, 0.0 });

  if (!loadGridFromFile(file_name, _grid)) {
    LogError() << "Can't load occupancy grid from file.";
    return false;
  }
  // std::cout << _grid << std::endl;
  Image out;
  francor::mapping::algorithm::occupancy::convertGridToImage(_grid, out);
  out.transformTo(ColourSpace::BGR);
  drawPoseOnImage(_ego.pose(), out);
  francor::vision::saveImageToFile("/tmp/occupancy_grid.png", out);

  if (!pipeline.initialize()) {
    LogError() << "Can't initialize \"" << pipeline.name() << "\" pipeline.";
    return false;
  }

  return true;
}

bool processStep(const Vector2d& delta_position)
{
  const Transform2d transform({ Angle::createFromDegree(-2.0) }, delta_position);

  _ego.setPose(transform * _ego.pose());
  
  auto start = std::chrono::system_clock::now();
  pipeline.input(PipeSimulateLaserScan::IN_SENSOR_POSE).assign(&_sensor_pose);

  if (!pipeline.process(_grid, _ego)) {
    LogError() << "Error occurred during processing of pipeline \"" << pipeline.name() << "\".";
    return false;
  }

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  LogDebug() << "processing time elapsed = " << elapsed.count() << " us";

  Image out;
  francor::mapping::algorithm::occupancy::convertGridToImage(_grid, out);
  out.transformTo(ColourSpace::BGR);
  drawPoseOnImage(_ego.pose(), out);
  drawLaserScanOnImage(pipeline.output(PipeSimulateLaserScan::OUT_SCAN).data<LaserScan>(), out);
  drawPointsOnImage(pipeline.output(PipeSimulateLaserScan::OUT_POINTS).data<Point2dVector>(), out);
  cv::imshow("occupancy grid", out.cvMat());
  cv::waitKey(10);
  francor::vision::saveImageToFile("/tmp/occupancy_grid.png", out);
  // std::cout << "result:" << std::endl << pipeline.output(PipeSimulateLaserScan::OUT_POINTS).data<Point2dVector>() << std::endl;
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