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

#include <algorithm>

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
void markLaserBeamBorder(GridType& grid,
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

/**
 * \brief Fills marked shapes in a grid. It uses the value used to mark the borders if this
 *        value is below given threshold.
 * \param grid The shapes of this grid will be filled. The grid must contain the shapes.
 * \param threshold The function fills the shapes with the value found at border if it is
 *        below this thresholds.
 */
template <class GridType>
void fillMarkedShapes(GridType& grid, const typename GridType::cell_type& threshold)
{
  typename GridType::cell_type current_value;
  typename GridType::cell_type default_value(grid.getDefaultCellValue());
  bool found_shape = false;

  for (std::size_t row = 0; row < grid.getNumCellsY(); ++row) {
    for (std::size_t col = 0; col < grid.getNumCellsX(); ++col) {
      auto& cell = grid(col, row);

      if (false == found_shape && cell != default_value && cell < threshold) {
        current_value = cell;
        found_shape = true;
      }
      else if (true == found_shape && cell == current_value) {
        current_value = default_value;
        found_shape = false;
      }
      
      if (true == found_shape && cell < threshold) {
        cell = current_value;
      }
    }
  }
}

template <class GridType>
void registerLaserBeam(GridType& grid,
                       const base::Point2d& origin,
                       const base::AnglePiToPi phi,
                       const base::Angle divergence,
                       const double distance,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied)
{
  using francor::algorithm::Ray2d;

  const base::Angle divergence_2 = divergence / 2.0;
  const double beam_width = distance * std::tan(divergence_2) * 2.0;
  const double cell_width = grid.getCellSize() / std::max(std::abs(std::cos(phi)), std::abs(std::sin(phi)));
  const std::size_t number_of_rays = static_cast<std::size_t>((beam_width / cell_width) + 2.0);
  const base::Angle phi_step = divergence / static_cast<double>(std::max(number_of_rays - 1, 1lu));
  const auto origin_idx_x = grid.getIndexX(origin.x());
  const auto origin_idx_y = grid.getIndexX(origin.y());

  std::cout << "beam width = " << beam_width << std::endl;
  std::cout << "cell width = " << cell_width << std::endl;
  std::cout << "num rays   = " << number_of_rays << std::endl;
  std::cout << "phi step   = " << phi_step << std::endl;
  std::cout << "phi        = " << phi << std::endl;

  if (number_of_rays % 2 == 0) {
    // start from the beam border and go to the middle
    base::AnglePiToPi current_phi = phi - divergence_2;

    for (std::size_t i = 0; i < number_of_rays; ++i, current_phi += phi_step) {
      // create a ray and walk through the map to set cells to free
      Ray2d ray_caster(Ray2d::create(origin_idx_x,
                                     origin_idx_y,
                                     grid.getNumCellsX(),
                                     grid.getNumCellsY(),
                                     grid.getCellSize(),
                                     origin,
                                     base::algorithm::line::calculateV(current_phi),
                                     distance));

      for (; ray_caster; ++ray_caster) {
        const auto idx = ray_caster.getCurrentIndex();
        grid(idx.x(), idx.y()) = cell_value_free;
      }                                      
      // set cell value to occupied if it wasn't set to free before
      if (cell_value_free != grid(ray_caster.getCurrentIndex().x(), ray_caster.getCurrentIndex().y())) {
        grid(ray_caster.getCurrentIndex().x(), ray_caster.getCurrentIndex().y()) = cell_value_occupied;
      }
    }
  }
  else {
    // place one in the middle and go the beam border
    base::AnglePiToPi current_phi = phi - divergence_2;

    for (std::size_t i = 0; i < number_of_rays; ++i, current_phi += phi_step) {
      Ray2d ray_caster(Ray2d::create(origin_idx_x,
                                     origin_idx_y,
                                     grid.getNumCellsX(),
                                     grid.getNumCellsY(),
                                     grid.getCellSize(),
                                     origin,
                                     base::algorithm::line::calculateV(current_phi),
                                     distance));

      for (; ray_caster; ++ray_caster) {
        const auto idx = ray_caster.getCurrentIndex();
        grid(idx.x(), idx.y()) = cell_value_free;
      }                                      
      // set cell value to occupied if it wasn't set to free before
      if (cell_value_free != grid(ray_caster.getCurrentIndex().x(), ray_caster.getCurrentIndex().y())) {
        grid(ray_caster.getCurrentIndex().x(), ray_caster.getCurrentIndex().y()) = cell_value_occupied;
      }
    }
  }
}

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor