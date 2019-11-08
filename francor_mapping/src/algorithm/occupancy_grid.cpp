/**
 * Algorithm and estimation function regarding occupancy grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */

#include "francor_mapping/algorihm/occupancy_grid.h"
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

      if (cell_value == OccupancyCell::UNKOWN) {
        image(row, col).gray() = pixel_value_unkown;
      }
      else if (cell_value == 0) {
        image(row, col).gray() = 255;
      }
      else {
        image(row, col).gray() = 100 - cell_value;
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
        grid(x, y).value = 0;
      }
      else if (pixel_value < 100) {
        grid(x, y).value = 100 - pixel_value;
      }
      else {
        grid(x, y).value = OccupancyCell::UNKOWN;
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

    for (const auto& idx : ray)
    {
      const auto cell_value = grid(idx.x(), idx.y()).value;

      if (cell_value > 0 && cell_value <= 100) {
        points.push_back( { static_cast<double>(idx.x()) * grid.getCellSize() + grid.getOrigin().x(),
                            static_cast<double>(idx.y()) * grid.getCellSize() + grid.getOrigin().y() } );

        break;
      }
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

    for (const auto& idx : ray)
    {
      const auto cell_value = grid(idx.x(), idx.y()).value;

      if (cell_value > 0 && cell_value <= 100) {
        Vector2d point{ static_cast<double>(idx.x()) * grid.getCellSize() + grid.getOrigin().x(),
                        static_cast<double>(idx.y()) * grid.getCellSize() + grid.getOrigin().y() };
        distances[beam] = (Vector2d(pose.position().x(), pose.position().y()) - point).norm();
        found_occupancy = true;
        break;
      }
    }                            

    if (!found_occupancy)
      distances[beam] = std::numeric_limits<double>::quiet_NaN();

    current_phi += phi_step;
  }

  scan = LaserScan(distances, pose, phi_min, phi_min + phi_step * num_beams, phi_step);

  return true;
}                                  



void updateGridCell(OccupancyCell& cell, const double distance, const double current_distance)
{
  if (current_distance < distance)
    cell.value = 0;
  else
    cell.value = 100;
}



void pushLaserScanToGrid(OccupancyGrid& grid, const base::LaserScan& laser_scan, const base::Pose2d& pose_ego)
{
  using francor::base::Point2d;
  using francor::base::Angle;
  using francor::base::Line;
  using francor::algorithm::Ray2d;

  Angle current_phi = laser_scan.phiMin();
  const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x() + pose_ego.position().x());
  const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y() + pose_ego.position().y());

  for (const auto& distance : laser_scan.distances())
  {
    const Point2d position = laser_scan.pose().position() + pose_ego.position();
    const Angle phi = current_phi + laser_scan.pose().orientation() + pose_ego.orientation();
    const auto direction = base::algorithm::line::calculateV(phi);

    Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.getNumCellsX(), grid.getNumCellsY(), grid.getCellSize(), position, direction, distance));
    std::size_t counter = 0;

    for (const auto& idx : ray)
    {
      const double current_distance = (grid.getCellPosition(idx.x(), idx.y()) - position).norm();
      updateGridCell(grid(idx.x(), idx.y()), distance, current_distance); // \todo replace constant value with tsd calculation function
      ++counter;
    }

    std::cout << "counter = " << counter << std::endl;
    current_phi += laser_scan.phiStep();
  }
}

} // end namespace occupancy

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor