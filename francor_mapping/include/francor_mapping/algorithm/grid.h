/**
 * Algorithm and estimation function regarding grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. November 2019
 */
#pragma once

#include <francor_base/laser_scan.h>

#include <francor_base/point.h>
#include <francor_base/size.h>
#include <francor_base/transform.h>
#include <francor_base/algorithm/math.h>

#include <francor_algorithm/shared_array.h>

#include <algorithm>

namespace francor {

namespace mapping {

namespace algorithm {

namespace grid {

/**
 * \brief Helper class for getting grid cell related sizes of class Grid. This class is a friend of Grid and
 *        makes the size attribues accessable.
 */
template <class GridType>
class SizeHandler
{
public:
  explicit SizeHandler(const GridType& grid) : _grid(grid) { }

  inline double size() const { return _grid._cell_size; }
  inline base::Size2u count() const {
    return static_cast<const francor::algorithm::SharedArray2d<typename GridType::cell_type>&>(_grid).size();
  }

private:
  const GridType& _grid;
};

/**
 * \brief Helper class and implementation of find algorithms to find cell related attributes like cell index for
 *        a given position. This class is a friend of Grid.
 */
template <class GridType>
class FindOperation
{
public:
  explicit FindOperation(const GridType& grid) : _grid(grid) { }

  class CellFindOperation {
  public:
    explicit CellFindOperation(const GridType& grid) : _grid(grid) { }

    inline base::Size2u index(const base::Point2d& position) const
    {
      return { static_cast<std::size_t>((position.x() + _grid.getOrigin().x()) / _grid.cell().size()),
               static_cast<std::size_t>((position.y() + _grid.getOrigin().y()) / _grid.cell().size()) };
    }
    inline base::Point2d position(const base::Size2u& cell_index) const
    {
      return { (static_cast<double>(cell_index.x()) + 0.5) * _grid.cell().size(),
               (static_cast<double>(cell_index.y()) + 0.5) * _grid.cell().size() };
    }

  private:
    const GridType& _grid;
  };

  inline CellFindOperation cell() const { return CellFindOperation(_grid); }

private:
  const GridType& _grid;
};

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
                         const typename GridType::cell_type& cell_value_occupied);


/**
 * \brief Fills marked shapes in a grid. It uses the value used to mark the borders if this
 *        value is below given threshold.
 * \param grid The shapes of this grid will be filled. The grid must contain the shapes.
 * \param threshold The function fills the shapes with the value found at border if it is
 *        below this thresholds.
 */
template <class GridType>
void fillMarkedShapes(GridType& grid, const typename GridType::cell_type& threshold);

template <class GridType>
void registerLaserBeam(GridType& grid,
                       const base::Point2d& origin,
                       const base::AnglePiToPi phi,
                       const double distance,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied);                                        

template <class GridType>
void registerLaserBeam(GridType& grid,
                       const base::Point2d& origin,
                       const base::AnglePiToPi phi,
                       const base::Angle divergence,
                       const double distance,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied);

template <class GridType>
void registerLaserScan(GridType& grid,
                       const base::Pose2d& ego_pose,
                       const base::LaserScan& scan,
                       const typename GridType::cell_type& cell_value_free,
                       const typename GridType::cell_type& cell_value_occupied);

} // end namespace grid

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor