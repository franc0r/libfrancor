#include <francor_base/log.h>
#include <francor_base/transform.h>

#include <francor_vision/image.h>
#include <francor_vision/io.h>

#include <francor_mapping/pipeline_occupancy_grid.h>
#include <francor_mapping/algorihm/occupancy_grid.h>
#include <francor_mapping/pipeline_stage_ego_object.h>

using francor::mapping::PipeSimulateLaserScan;
using francor::mapping::OccupancyGrid;
using francor::mapping::EgoObject;
using francor::vision::Image;
using francor::base::Pose2d;
using francor::base::Point2dVector;
using francor::base::Transform2d;
using francor::base::LogError;
using francor::base::LogInfo;
using francor::base::Vector2d;

OccupancyGrid _grid;
EgoObject _ego;
const Pose2d _sensor_pose({ 0.0, 0.0 }, 0.0);

PipeSimulateLaserScan pipeline;

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

  if (!pipeline.initialize()) {
    LogError() << "Can't initialize \"" << pipeline.name() << "\" pipeline.";
    return false;
  }

  return true;
}

bool processStep(const Vector2d& delta_position)
{
  const Transform2d transform({ 0.0 }, delta_position);

  _ego.setPose(transform * _ego.pose());
  pipeline.input(PipeSimulateLaserScan::IN_SENSOR_POSE).assign(&_sensor_pose);

  if (!pipeline.process(_grid, _ego)) {
    LogError() << "Error occurred during processing of pipeline \"" << pipeline.name() << "\".";
    return false;
  }

  std::cout << "result:" << std::endl << pipeline.output(PipeSimulateLaserScan::OUT_POINTS).data<Point2dVector>() << std::endl;
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
  }

  return 0;
}