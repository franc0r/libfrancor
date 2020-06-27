#include "francor_mapping/grid.h"
#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/tsd_grid.h"

namespace francor {

namespace mapping {

template <typename CellType>
Grid<CellType>::Grid(Grid&& origin)
{
  this->operator=(std::move(origin));
}

template <typename CellType>
Grid<CellType>::Grid(Grid& origin, const base::Rect2u& roi)
  : _cell_size(origin._cell_size),
    _size_x(roi.width() * _cell_size),
    _size_y(roi.height() * _cell_size),
    _origin(origin.getCellPosition(roi.x(), roi.y())),
    _data(origin._data, roi)
{
  if (!_data.isInitialized()) {
    base::Log<base::LogLevel::ERROR, base::LogGroup::ALGORITHM, Grid::class_name>() << "grid data memory is not initialized. "
      << "This can happen, if ROI is out of range. Grid will be reseted.";
    this->clear();
  }
}

template <typename CellType>
bool Grid<CellType>::init(const std::size_t numCellsX, const std::size_t numCellsY, const double cellSize)
{
  if (cellSize <= std::numeric_limits<double>::min())
  {
    using Log = base::Log<base::LogLevel::ERROR, base::LogGroup::ALGORITHM, Grid::class_name>;
    Log() << "Grid: cell size must be greater than zero. Can't initialize grid. cell size = " << cellSize << ".";
    return false;
  }

  // allocate grid data
  _data.resize(numCellsX, numCellsY);
  _cell_size = cellSize;

  // calculate size from other parameter
  _size_x = _data.cols() * _cell_size;
  _size_y = _data.rows() * _cell_size;

  return true;
}

template class Grid<OccupancyCell>;
template class Grid<TsdCell>;
template class Grid<double>;
template class Grid<float>;
template class Grid<int>;

} // end namespace mapping

} // end namespace francor