/**
 * Algorithm and estimation function regarding grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_base/laser_scan.h>
#include <francor_base/log.h>
#include <francor_base/point.h>
#include <francor_base/algorithm/math.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/grid.h"

namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

/**
 * \brief Marks the border of the laser beam in a map with given value.
 * 
 * \param grid The laser beam border will be marked in this map.
 * \param phi Absolute orientation in map of the laser beam.
 * \param distance The distance of the laser measurement.
 * \param cell_value This value will be used to draw the border.
 */
template <class GridType>
inline void markLaserBeamBorder(GridType& grid,
                                const base::Point2d& origin,
                                const base::AnglePiToPi phi,
                                const base::Angle divergence,
                                const double distance,
                                const typename GridType::cell_type& cell_value_free,
                                const typename GridType::cell_type& cell_value_occupied)
{
  using francor::algorithm::Ray2d;
  using francor::base::LogError;

  const auto divergence_2 = divergence / 2.0;
  const auto direction_upper = base::algorithm::line::calculateV(phi + divergence_2);
  const auto direction_lower = base::algorithm::line::calculateV(phi - divergence_2);
  const auto origin_idx_x = grid.getIndexX(origin.x());
  const auto origin_idx_y = grid.getIndexY(origin.y());

  // create two rays for upper and lower border
  Ray2d upper_border(Ray2d::create(origin_idx_x,
                                   origin_idx_y,
                                   grid.getNumCellsX(),
                                   grid.getNumCellsY(),
                                   grid.getCellSize(),
                                   origin,
                                   direction_upper,
                                   distance));
  Ray2d lower_border(Ray2d::create(origin_idx_x,
                                   origin_idx_y,
                                   grid.getNumCellsX(),
                                   grid.getNumCellsY(),
                                   grid.getCellSize(),
                                   origin,
                                   direction_lower,
                                   distance));

  // iterate along rays and mark grid cell with given value
  // the final indices of both rays will be used for last border
  for (; upper_border; ++upper_border) {
    grid(upper_border.getCurrentIndex().x(), upper_border.getCurrentIndex().y()) = cell_value_free;
  }
  for (; lower_border; ++lower_border) {
    grid(lower_border.getCurrentIndex().x(), lower_border.getCurrentIndex().y()) = cell_value_free;
  }

  // create last ray to close the shape
  const auto upper_end_idx_x = upper_border.getCurrentIndex().x();
  const auto upper_end_idx_y = upper_border.getCurrentIndex().y();
  const auto upper_end_point = grid.getCellPosition(upper_end_idx_x, upper_end_idx_y);
  const auto lower_end_point = grid.getCellPosition(lower_border.getCurrentIndex().x(), lower_border.getCurrentIndex().y());
  const double distance_head = (lower_end_point - upper_end_point).norm() + grid.getCellSize() * 0.9;

  Ray2d head_border(Ray2d::create(upper_end_idx_x,
                                  upper_end_idx_y,
                                  grid.getNumCellsX(),
                                  grid.getNumCellsY(),
                                  grid.getCellSize(),
                                  upper_end_point,
                                  base::algorithm::line::calculateV(lower_end_point, upper_end_point),
                                  distance_head));

  for (; head_border; ++head_border) {
    grid(head_border.getCurrentIndex().x(), head_border.getCurrentIndex().y()) = cell_value_occupied;
  }

  // check if end points are equal otherwise something went wrong
  // if (lower_border.getCurrentIndex() != head_border.getCurrentIndex()) {
  //   LogError() << "markLaserBeamBorder(): end point of lower and head border are different. This shouldn't happend!";
  // }
}                         

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor