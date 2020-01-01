/**
 * Algorithm and estimation function regarding occupancy grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <cstddef>

#include <francor_base/point.h>
#include "francor_mapping/occupancy_grid.h"
namespace francor {

namespace base {
class Pose2d;
class Angle;
class LaserScan;
}

namespace vision {
class Image;
}

namespace mapping {

class OccupancyGrid;
struct OccupancyCell;

namespace algorithm {

namespace occupancy {

/**
 * \brief Converts an occupancy grid to an image. The resulting image is an gray scaled with following colour mapping:
 *          0% ->   0 (black)
 *        100% -> 100 (lite gray)
 *        nan  -> 200 (dark gray)
 * 
 * \param grid The input occupancy grid.
 * \param image The resulting gray scaled image.
 * \return true if convertion was successful.
 */
bool convertGridToImage(const OccupancyGrid& grid, vision::Image& image);

/**
 * \brief Creates an occupancy grid from an image. The image type must be gray sacled. A pixel will represents a
 *        grid cell with following value mapping:
 *              0  ->     0% (free)
 *            100  ->   100% (fully occupied)
 *        ~[0-100] -> unkown
 * 
 * \param image The input gray scaled image.
 * \param cell_size The grid cell size of the resulting occupancy grid.
 * \param grid The resulting occupancy grid.
 * \return true if the creating was successful.
 */
bool createGridFromImage(const vision::Image& image, const double cell_size, OccupancyGrid& grid);

/**
 * \brief Reconstruct points from grid at given pose.
 * 
 * \param grid The occupancy grid used to reconstruct.
 * \param pose Pose the laser sensor.
 * \param phi_min Start angle (left).
 * \param phi_step Angle steps between each laser beam.
 * \param num_beams Numbers of laser beams.
 * \param range Range of the laser sensor.
 * \param points Reconstructed points.
 * \return true if reconstruction was successful.
 */
bool reconstructPointsFromGrid(const OccupancyGrid& grid, const base::Pose2d& pose, const base::Angle phi_min,
                               const base::Angle phi_step, const std::size_t num_beams, const double range,
                               base::Point2dVector& points);

/**
 * \brief Reconstruct a laser scan from grid at given pose.
 * 
 * \param grid The occupancy grid used to reconstruct.
 * \param pose_ego Pose the ego object.
 * \param pose_sensor Pose of the sensor in ego frame.
 * \param phi_min Start angle (left).
 * \param phi_step Angle steps between each laser beam.
 * \param num_beams Numbers of laser beams.
 * \param range Range of the laser sensor.
 * \param scan Reconstructed laser scan.
 * \return true if reconstruction was successful.
 */
bool reconstructLaserScanFromGrid(const OccupancyGrid& grid, const base::Pose2d& pose_ego, const base::Pose2d& pose_sensor,
                                  const base::Angle phi_min, const base::Angle phi_step, const std::size_t num_beams,
                                  const double range, base::LaserScan& scan);

/**
 * \brief Updates a occupancy grid cell using formula cell = (value / (1 - value)) * old.value. NOTE: POC!
 * 
 * \param cell Occupancy grid cell.
 * \param value New input value.
 */
struct updateGridCell
{
  constexpr updateGridCell(OccupancyCell& cell, const float value)
  {
    if (std::isnan(cell.value)) {
      cell.value = value;
    }
    else {
      //                  P(B|A) * P(A)
      // P(A|B) = -------------------------------
      //          P(B|A) * P(A) + P(B|~A) * P(~A)
      cell.value = (value * cell.value) / (value * cell.value + (1.0f - value) * (1.0f - cell.value));
    }
  }
};

/**
 * \brief Pushes a laser scan into a occupancy grid using the update grid cell function.
 * 
 * \param grid Occupancy grid.
 * \param scan Input laser scan. To the ego pose will be added to the scan pose.
 * \param pose_ego Input ego pose.
 * \param normals Normals of the resulting laser scan points. Each normal corresponds with the beam of same index. If no normals are given the beam angle will be
 *                used instead.
 */
void pushLaserScanToGrid(OccupancyGrid& grid, const base::LaserScan& scan, const base::Pose2d& pose_ego,
                         const std::vector<base::NormalizedAngle>& normals = std::vector<base::NormalizedAngle>());

/**
 * \brief Push points of a laser scan into a occupancy grid using the update grid cell function.
 * 
 * \param grid Occupancy grid.
 * \param points Input points of a laser scan. To the ego pose will be added to the points.
 * \param pose_ego Ego pose used as start point for each ray.
 * \param normals Normals of the input points. Each normal corresponds with the point of same index.
 * \return true if push was successfull.
 */
bool pushPointsToGrid(OccupancyGrid& grid, const base::Point2dVector& points, const base::Pose2d& pose_ego, const std::vector<base::NormalizedAngle>& normals);

/**
 * \brief Pushes a laser point (endpoint of the laser beam) into a occupancy grid. The shape of the point is
 *        according gaussian distribution.
 * 
 * \param grid Occupancy grid.
 * \param x X coordinate on grid.
 * \param y y coordinate on grid.
 * \param point_size Size of the laser point in grid cells.
 */
void pushLaserPointToGrid(OccupancyGrid& grid, const std::size_t x, const std::size_t y, const std::size_t point_size, const base::Angle point_yaw);

} // end namespace occupancy

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor