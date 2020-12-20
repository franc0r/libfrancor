
#include <francor_base/log.h>

#include <francor_algorithm/ray_caster_2d.h>

#include "francor_mapping/algorithm/grid.h"
#include "francor_mapping/grid.h"
#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/tsd_grid.h"

namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

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
  const auto origin_idx = grid.find().cell().index(origin);

  // create two rays for upper and lower border
  Ray2d upper_border(Ray2d::create(origin_idx.x(),
                                   origin_idx.y(),
                                   grid.cell().count().x(),
                                   grid.cell().count().y(),
                                   grid.cell().size(),
                                   origin,
                                   direction_upper,
                                   distance));
  Ray2d lower_border(Ray2d::create(origin_idx.x(),
                                   origin_idx.y(),
                                   grid.cell().count().x(),
                                   grid.cell().count().y(),
                                   grid.cell().size(),
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
  const auto upper_end_point = grid.find().cell().position(upper_border.getCurrentIndex());
  const auto lower_end_point = grid.find().cell().position(lower_border.getCurrentIndex());
  const double distance_head = (lower_end_point - upper_end_point).norm() + grid.cell().size() * 0.9;

  Ray2d head_border(Ray2d::create(upper_end_idx_x,
                                  upper_end_idx_y,
                                  grid.cell().count().x(),
                                  grid.cell().count().y(),
                                  grid.cell().size(),
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


template <class GridType>
void fillMarkedShapes(GridType& grid, const typename GridType::cell_type& threshold)
{
  typename GridType::cell_type current_value;
  typename GridType::cell_type default_value(grid.getDefaultCellValue());
  bool found_shape = false;

  for (std::size_t row = 0; row < grid.cell().count().y(); ++row) {
    for (std::size_t col = 0; col < grid.cell().count().x(); ++col) {
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
                       const double distance,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied)
{
  using francor::algorithm::Ray2d;

  // create a ray and walk through the map to set cells to free
  const auto origin_idx = grid.find().cell().index(origin);
  Ray2d ray_caster(Ray2d::create(origin_idx.x(),
                                 origin_idx.y(),
                                 grid.cell().count().x(),
                                 grid.cell().count().y(),
                                 grid.cell().size(),
                                 origin,
                                 base::algorithm::line::calculateV(phi),
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
  const double cell_width = grid.cell().size() / std::max(std::abs(std::cos(phi)), std::abs(std::sin(phi)));
  const std::size_t number_of_rays = (divergence == 0.0 ?
                                      1 :
                                      static_cast<std::size_t>((beam_width / cell_width) + 2.0));
  const base::Angle phi_step = divergence / static_cast<double>(std::max(number_of_rays - 1, 1lu));


  // std::cout << "beam width = " << beam_width << std::endl;
  // std::cout << "cell width = " << cell_width << std::endl;
  // std::cout << "num rays   = " << number_of_rays << std::endl;
  // std::cout << "phi step   = " << phi_step << std::endl;
  // std::cout << "phi        = " << phi << std::endl;

  // start from the beam border and go to the middle
  base::AnglePiToPi current_phi = phi - divergence_2;

  for (std::size_t i = 0; i < number_of_rays; ++i, current_phi += phi_step) {
    registerLaserBeam(grid, origin, current_phi, distance, cell_value_free, cell_value_occupied);
  }
}


template <class GridType>
void registerLaserScan(GridType& grid,
                       const base::Pose2d& ego_pose,
                       const base::LaserScan& scan,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied)
{
  using francor::base::Transform2d;
  using francor::base::Pose2d;
  using francor::base::AnglePiToPi;

  const Transform2d tranform({ ego_pose.orientation() },
                             { ego_pose.position().x(), ego_pose.position().y() });
  const Pose2d pose(tranform * scan.pose());
  AnglePiToPi current_phi = pose.orientation() + scan.phiMin();

  for (const auto distance : scan.distances()) {
    registerLaserBeam(grid, pose.position(), current_phi, scan.divergence(), distance, cell_value_free, cell_value_occupied);
    current_phi += scan.phiStep();
  }
}

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor
