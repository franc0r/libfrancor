/**
 * Algorithm and estimation function regarding occupancy grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <cstddef>

#include <francor_base/point.h>

namespace francor {

namespace base {
class Pose2d;
class Angle;
}

namespace vision {
class Image;
}

namespace mapping {

class OccupancyGrid;

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

} // end namespace occupancy

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor