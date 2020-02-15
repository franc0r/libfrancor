/**
 * Algorithm and estimation function regarding occupancy grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/algorithm/grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"
#include "francor_mapping/occupancy_grid.h"

#include <francor_base/log.h>
#include <francor_base/pose.h>
#include <francor_base/angle.h>
#include <francor_base/line.h>
#include <francor_base/laser_scan.h>
#include <francor_base/vector.h>
#include <francor_base/transform.h>

#include <francor_algorithm/ray_caster_2d.h>

#include <francor_vision/image.h>

#include <algorithm>

namespace francor {

namespace mapping {

namespace algorithm {

namespace occupancy {

using francor::base::LogError;
using francor::vision::Image;
using francor::vision::ColourSpace;

bool convertGridToImage(const OccupancyGrid& grid, vision::Image& image)
{
  constexpr std::uint8_t pixel_value_unkown = 200;

  image.resize(grid.getNumCellsX(), grid.getNumCellsY(), ColourSpace::GRAY);

  for (std::size_t col = 0; col < image.cols(); ++col) {
    for (std::size_t row = 0; row < image.rows(); ++row) {
      const auto cell_value = grid(col, row).value;

      if (std::isnan(cell_value)) {
        image(row, col).gray() = pixel_value_unkown;
      }
      else if (cell_value <= 0.1f) {
        image(row, col).gray() = 255;
      }
      else {
        image(row, col).gray() = (100 - static_cast<std::uint8_t>(cell_value * 100.0f)) * 2;
      }
    }
  }

  return true;
}

bool createGridFromImage(const Image& image, const double cell_size, OccupancyGrid& grid)
{
  if (!grid.init(image.cols(), image.rows(), cell_size)) {
    LogError() << "createGridFromImage(): error occurred during occupancy grid initialization. Can't create grid.";
    grid.clear();
    return false;
  }

  for (std::size_t x = 0; x < grid.getNumCellsX(); ++x) {
    for (std::size_t y = 0; y < grid.getNumCellsY(); ++y) {
      const auto pixel_value = image(y, x).gray();

      if (pixel_value == 255) {
        grid(x, y).value = 0.1f;
      }
      else if (pixel_value < 100) {
        grid(x, y).value = static_cast<float>(100 - pixel_value) / 100.0f;
      }
      else {
        grid(x, y).value = std::numeric_limits<float>::quiet_NaN();
      }
    }
  }

  return true;
}

bool reconstructPointsFromGrid(const OccupancyGrid& grid, const base::Pose2d& pose, const base::Angle phi_min,
                               const base::Angle phi_step, const std::size_t num_beams, const double range,
                               base::Point2dVector& points)
{
  using francor::base::Point2d;
  using francor::base::Point2dVector;
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;
  using francor::base::Vector2i;  

  Angle current_phi = pose.orientation() + phi_min;
  const std::size_t start_index_x = grid.getIndexX(pose.position().x());
  const std::size_t start_index_y = grid.getIndexY(pose.position().y());

  points.resize(0);
  points.reserve(num_beams);

  for (std::size_t beam = 0; beam < num_beams; ++beam)
  {
    const auto direction = base::algorithm::line::calculateV(current_phi);
    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(),
                            grid.getNumCellsY(), grid.getCellSize(), pose.position(), direction, range));
    double best_value = 0.5;
    Vector2i best_index;
    bool found_obstacle = false;

    for (const auto& idx : ray)
    {
      const auto cell_value = grid(idx.x(), idx.y()).value;

      if (found_obstacle || (cell_value > 0.75 && cell_value <= 1.0)) {
        if (cell_value > best_value) {
          best_value = cell_value;
          best_index = idx;
          found_obstacle = true;
        }
        else
        {
          break;
        }
      }
    }                            

    if (found_obstacle) {
      points.push_back( { (static_cast<double>(best_index.x()) + 0.5) * grid.getCellSize() + grid.getOrigin().x(),
                          (static_cast<double>(best_index.y()) + 0.5) * grid.getCellSize() + grid.getOrigin().y() } );
    }

    current_phi += phi_step;
  }

  return true;
}


bool reconstructLaserScanFromGrid(const OccupancyGrid& grid, const base::Pose2d& pose_ego, const base::Pose2d& pose_sensor,
                                  const base::Angle phi_min, const base::Angle phi_step, const std::size_t num_beams,
                                  const double range, base::LaserScan& scan)
{
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;
  using francor::base::LaserScan;
  using francor::base::Vector2d;
  using francor::base::Vector2i;
  using francor::base::Pose2d;
  using francor::base::Transform2d;

  const Transform2d transform({ pose_ego.orientation() }, { pose_ego.position().x(), pose_ego.position().y() });
  const Pose2d pose(transform * pose_sensor);

  Angle current_phi = pose.orientation() + phi_min;
  const std::size_t start_index_x = grid.getIndexX(pose.position().x());
  const std::size_t start_index_y = grid.getIndexY(pose.position().y());

  std::vector<double> distances;
  distances.resize(num_beams);

  for (std::size_t beam = 0; beam < num_beams; ++beam)
  {
    const auto direction = base::algorithm::line::calculateV(current_phi);
    bool found_occupancy = false;
    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(),
                            grid.getNumCellsY(), grid.getCellSize(), pose.position(), direction, range));
    double best_value = 0.5;
    Vector2i best_index;

    for (const auto& idx : ray)
    {
      const auto cell_value = grid(idx.x(), idx.y()).value;

      if (found_occupancy || (cell_value > 0.75 && cell_value <= 1.0)) {
        if (cell_value > best_value) {
          best_value = cell_value;
          best_index = idx;
          found_occupancy = true;
        }
        else
        {
          break;          
        }
      }
    }                            

    if (found_occupancy) {
      Vector2d point{ (static_cast<double>(best_index.x()) + 0.5) * grid.getCellSize() + grid.getOrigin().x(),
                      (static_cast<double>(best_index.y()) + 0.5) * grid.getCellSize() + grid.getOrigin().y() };
      distances[beam] = (Vector2d(pose.position().x(), pose.position().y()) - point).norm();
    }
    else {
      distances[beam] = std::numeric_limits<double>::quiet_NaN();
    }

    current_phi += phi_step;
  }

  scan = LaserScan(distances, pose_sensor, phi_min, phi_min + phi_step * num_beams, phi_step, range);

  return true;
}                                  

void pushLaserScanToGrid(OccupancyGrid& grid, const base::LaserScan& laser_scan, const base::Pose2d& pose_ego, const std::vector<base::NormalizedAngle>& normals)
{
  using francor::base::Point2d;
  using francor::base::Vector2d;
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;

  Angle current_phi = laser_scan.phiMin();
  const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x() + pose_ego.position().x());
  const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y() + pose_ego.position().y());
  const Point2d position = laser_scan.pose().position() + pose_ego.position();
  std::size_t index_normal = 0;

  // process each distance measurement. start from phi min
  for (std::size_t i = 0; i < laser_scan.distances().size(); ++i)
  {
    // assert for consitent laser scan
    assert(laser_scan.distances().size() == laser_scan.pointExpansions().size());

    const double point_expansion = laser_scan.pointExpansions()[i];
    const double distance = laser_scan.distances()[i];
    const Angle phi = current_phi + laser_scan.pose().orientation() + pose_ego.orientation();
    const auto direction = base::algorithm::line::calculateV(phi);
    const auto distance_corrected = (std::isnan(distance) || std::isinf(distance) ?
                                     laser_scan.range() :
                                     distance - 0.125); //point_expansion * 0.5); \todo parameter for 0.125 

    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(),
              grid.getNumCellsY(), grid.getCellSize(), position, direction, distance_corrected));
    std::size_t counter = 0;

    // free every grid cell that is intersected by the ray by 30%
    for (const auto& idx : ray)
    {
      updateGridCell(grid(idx.x(), idx.y()), 0.35f);
      ++counter;
    }

    // std::cout << "counter = " << counter << std::endl;
    // do only if distance measurement is valid

    if (!(std::isnan(distance) || std::isinf(distance)))
    {
      // add a special propability for the ray end point (actually the measurement)
      const auto end_position(position + direction * distance);
      const std::size_t end_index_x = grid.getIndexX(end_position.x());
      const std::size_t end_index_y = grid.getIndexY(end_position.y());
      // minium one cell is needed
      const std::size_t cells = static_cast<std::size_t>(std::max(1.0, point_expansion / grid.getCellSize())); 
      const auto angle = index_normal < normals.size() ? normals[index_normal++] + pose_ego.orientation(): phi;
      pushLaserPointToGrid(grid, end_index_x, end_index_y, cells % 2 ? cells : cells + 1, angle);

    }

    current_phi += laser_scan.phiStep();
  }
}

bool pushPointsToGrid(OccupancyGrid& grid, const base::Point2dVector& points, const base::Pose2d& pose_ego, const std::vector<base::NormalizedAngle>& normals)
{
  if (points.size() != normals.size()) {
    base::LogError() << "pushPointsToGrid(): number of points and normals isn't equal. Cancel push to grid.";
    return false;
  }

  using francor::base::Line;

  const std::size_t start_index_x = grid.getIndexX(pose_ego.position().x());
  const std::size_t start_index_y = grid.getIndexY(pose_ego.position().y());
  // const 
}

void pushLaserPointToGrid(OccupancyGrid& grid, const std::size_t x, const std::size_t y, const std::size_t point_size, const base::Angle point_yaw)
{
  grid::pushPoint<OccupancyGrid, updateGridCell, 5>(grid, x, y, point_size, point_yaw);
}


} // end namespace occupancy

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor