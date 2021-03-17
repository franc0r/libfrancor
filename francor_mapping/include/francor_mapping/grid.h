/**
 * \brief This abstract base class that represents a 2d grid. Note: POC
 */
#pragma once

#include <limits>

#include <francor_base/log.h>
#include <francor_base/point.h>
#include <francor_base/size.h>

#include <francor_algorithm/array_data_access.h>
#include <francor_algorithm/shared_array.h>

#include "francor_mapping/algorithm/grid.h"

namespace francor {

namespace mapping {

template<typename Cell>
class Grid : protected francor::algorithm::SharedArray2d<Cell>
{
public:
  using cell_type = Cell;
  using francor::algorithm::SharedArray2d<Cell>::operator();
  using francor::algorithm::SharedArray2d<Cell>::row;
  using francor::algorithm::SharedArray2d<Cell>::col;

  /**
   * \brief Default constructor. Constructs an empty invalid grid.
   */
  Grid() = default;
  Grid(const Grid&) = default;
  /**
   * \brief Moves all context to this and resets the origin grid object.
   */
  Grid(Grid&& origin)
  {
    this->operator=(std::move(origin));
  }
  /**
   * \brief Constructs with shared content and ROI.
   * \param rhs The source grid object.
   * \param roi The ROI of this object. The grid content is limited to the ROI.
   */
  Grid(const Grid& rhs, const base::Rectu& roi)
    : francor::algorithm::SharedArray2d<Cell>(rhs, roi),
      _cell_size(rhs._cell_size),
      _size(_cell_size * roi.size().x(), _cell_size * roi.size().y()),
      _origin(rhs._origin),
      _default_cell_value(rhs._default_cell_value)
  { }
  /**
   * \brief Defaulted destructor.
   */
  virtual ~Grid() = default;
  /**
   * \brief Defaulted copy assignment operator.
   */
  Grid& operator=(const Grid&) = default;
  /**
   * \brief Moves all context to this and resets the origin grid object.
   */
  Grid& operator=(Grid&& origin)
  {
    francor::algorithm::SharedArray2d<Cell>::operator=(origin);
    _cell_size = origin._cell_size;
    _size      = origin._size;

    _default_cell_value = origin._default_cell_value;
    origin.clear();

    return *this;
  }
  /**
   * \brief Initialize this grid using the given arguments. The size is given in number of cells in x and y direction.
   * 
   * \param grid_size Size of the grid in number of cells.
   * \param cell_size Size (edge length) of each cell in meter.
   * \param initial_cell_value Each cell is initialized with this value.
   * \return true if grid was successfully initialized.
   */
  bool init(const base::Size2u grid_size, const double cell_size, const cell_type& initial_cell_value = {})
  {
    if (cell_size <= std::numeric_limits<double>::min())
    {
      base::LogError() << "Grid: cell size must be greater than zero. Can't initialize grid. cell size = " << cell_size;
      return false;
    }

    // allocate grid data
    francor::algorithm::SharedArray2d<Cell>::resize(grid_size, initial_cell_value);
    _default_cell_value = initial_cell_value;
    _cell_size = cell_size;

    // calculate size from other parameter
    const auto& allocated_size = francor::algorithm::SharedArray2d<Cell>::size();
    _size = { allocated_size.x() * _cell_size, allocated_size.y() * _cell_size };

    return true;
  }
  /**
   * \brief Resets this grid. The grid will be empty and invalid.
   */
  void clear()
  {
    francor::algorithm::SharedArray2d<Cell>::clear();
    _cell_size = 0.0;
    _size = {0.0, 0.0};
  }
  /**
   * \brief Checks if this grid is empty.
   * 
   * \return true if grid is empty.
   */
  bool isEmpty() const
  {
    return francor::algorithm::SharedArray2d<Cell>::size().x() == 0u
           ||
           francor::algorithm::SharedArray2d<Cell>::size().y() == 0u;
  }
  /**
   * \brief Checks if this grid is valid. The cell size must be greater than zero and the grid size min 1x1.
   * 
   * \return true if grid is valid.
   */
  bool isValid() const { return _cell_size > 0.0 && !isEmpty(); }
  /**
   * \brief Return a cell size representation that holds the number of cells and cell size. For example to
   *        get the grid cell size this call is used:
   * 
   *                           grid.cell().size()
   * 
   *        or to get the number of cells of this grid:
   * 
   *                           grid.cell().count()
   * 
   * \return Cell size representation.
   */
  algorithm::grid::SizeHandler<Grid> cell() const { return algorithm::grid::SizeHandler<Grid>(*this); }
  /**
   * \brief Return the current grid size in meter.
   * \return Current grid size in meter.
   */
  inline const base::Size2d& size() const { return _size; }
  /**
   * \brief Starts a search to find attributes or characteristics of this grid, like an index of a grid cell.
   *        Usually it is used in that way: grid.find().[search topic]().[what is searched](). For example to
   *        find an index of a grid cell the function calls look like:
   * 
   *                           grid.find().cell().index([position input as point])
   *      
   *        or to find the position of a grid cell:
   * 
   *                           grid.find().cell().position([grid cell index])
   * 
   * \return A helper class that provides a set of find operations.
   */
  inline algorithm::grid::FindOperation<Grid> find() const { return algorithm::grid::FindOperation<Grid>(*this); }
  /**
   * \brief Returns the origin of this grid. The origin is located at cell (0, 0).
   * \return Origin coordinate in meter.
   */
  inline const base::Point2d& getOrigin() const noexcept { return _origin; }
  /**
   * \brief Return the default value what was used during initialization.
   * \return default grid cell value.
   */
  inline const Cell& getDefaultCellValue() const noexcept { return _default_cell_value; }

private:
  friend algorithm::grid::SizeHandler<Grid>;
  friend algorithm::grid::FindOperation<Grid>;
  friend typename algorithm::grid::FindOperation<Grid>::CellFindOperation;

  double _cell_size = 0.0;         //> size of each cell
  base::Size2d _size{0.0, 0.0};    //> size in m 
  base::Point2d _origin{0.0, 0.0}; //> origin coordinate of this grid in meter

  Cell _default_cell_value;
};

} // end namespace mapping

} // end namespace francor