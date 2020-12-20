/**
 * \brief This abstract base class that represents a 2d grid. Note: POC
 */
#pragma once

#include <limits>

#include <francor_base/log.h>
#include <francor_base/point.h>
#include <francor_base/size.h>

#include <francor_algorithm/array_data_access.h>

#include "francor_mapping/algorithm/grid.h"

namespace francor {

namespace mapping {

template<typename Cell>
class Grid
{
public:
  using cell_type = Cell;

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
    _data = std::move(origin._data);

    _grid_size = origin._grid_size;
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
    _data.resize(grid_size.x() * grid_size.y(), initial_cell_value);
    _default_cell_value = initial_cell_value;

    _grid_size = grid_size;
    _cell_size = cell_size;

    // calculate size from other parameter
    _size = {_grid_size.x() * _cell_size, grid_size.y() * _cell_size};

    return true;
  }


  /**
   * \brief Resets this grid. The grid will be empty and invalid.
   */
  void clear(void)
  {
    _grid_size = {0, 0};
    _cell_size = 0.0;
    _size = {0.0, 0.0};

    _data.clear();
  }

  /**
   * \brief Checks if this grid is empty.
   * 
   * \return true if grid is empty.
   */
  bool isEmpty() const { return _grid_size.x() == 0 || _grid_size.y() == 0; }
  /**
   * \brief Checks if this grid is valid. The cell size must be greater than zero and the grid size min 1x1.
   * 
   * \return true if grid is valid.
   */
  bool isValid() const { return _cell_size > 0.0 && _grid_size.x() > 0 && _grid_size.y() > 0; }
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
  algorithm::grid::SizeHandler<Grid> cell() const { return {*this}; }

  /**
   * \brief Return the current grid size in meter.
   * \return Current grid size in meter.
   */
  inline const base::Size2d& size() const { return _size; }

  /**
   * \brief Operator to access the cell at the given coordinates.
   * 
   * \return reference to the cell.
   */
  inline Cell& operator()(const std::size_t x, const std::size_t y) { return _data[y * _grid_size.x() + x]; }
  inline const Cell& operator()(const std::size_t x, const std::size_t y) const { return _data[y * _grid_size.x() + x]; }

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
  inline algorithm::grid::FindOperation<Grid> find() const { return { *this }; }

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


  francor::algorithm::ArrayDataAccess1D<Cell, 1u> row(const std::size_t index)
  {
    return francor::algorithm::ArrayDataAccess1D<Cell, 1u>{&_data[index * _grid_size.x()], _grid_size.x()};
  }
  francor::algorithm::ArrayDataAccess1D<const Cell, 1u> row(const std::size_t index) const
  {
    return francor::algorithm::ArrayDataAccess1D<const Cell, 1u>{&_data[index * _grid_size.x()], _grid_size.x()};
  }

private:
  friend algorithm::grid::SizeHandler<Grid>;
  friend algorithm::grid::FindOperation<Grid>;
  friend typename algorithm::grid::FindOperation<Grid>::CellFindOperation;

  base::Size2u _grid_size{0, 0};   //> grid size in number of cells
  double _cell_size = 0.0;         //> size of each cell
  base::Size2d _size{0.0, 0.0};    //> size in m 
  base::Point2d _origin{0.0, 0.0}; //> origin coordinate of this grid in meter

  Cell _default_cell_value;
  std::vector<Cell> _data;     //> grid cells
};

} // end namespace mapping

} // end namespace francor