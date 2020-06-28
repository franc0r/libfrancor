#include "francor_mapping/grid.h"


namespace francor {

namespace mapping {

template <typename CellType>
GridDataMemoryHandler<CellType>::GridDataMemoryHandler(const std::size_t numCellsX, const std::size_t numCellsY)
{
  this->resize(numCellsX, numCellsY);
}

template <typename CellType>
GridDataMemoryHandler<CellType>::GridDataMemoryHandler(GridDataMemoryHandler& handler, const base::Rect2u& roi)
{
  this->clear();
  this->shareGridDataFrom(handler, roi);
}

template <typename CellType>
GridDataMemoryHandler<CellType>::GridDataMemoryHandler(GridDataMemoryHandler&& origin)
{
  *this = std::move(origin);
}

template <typename CellType>
GridDataMemoryHandler<CellType>& GridDataMemoryHandler<CellType>::operator=(GridDataMemoryHandler&& origin)
{
  _is_shared_memory = origin._is_shared_memory;

  _data   = std::move(origin._data);
  _offset = origin._offset;
  _cols   = origin._cols;
  _rows   = origin._rows;
  _stride = origin._stride;

  origin.clear();

  return *this;
}

template <typename CellType>
void GridDataMemoryHandler<CellType>::clear()
{
  _is_shared_memory = false;
    
  _data   = nullptr;
  _offset = {0, 0};
  _cols   = 0;
  _rows   = 0;
  _stride = 0;
}

template <typename CellType>
void GridDataMemoryHandler<CellType>::resize(const std::size_t num_cells_x, const std::size_t num_cells_y)
{
  using LogWarn = base::Log<base::LogLevel::WARNING, base::LogGroup::ALGORITHM, class_name>;

  if (_is_shared_memory) {
    LogWarn() << "grid data memory was shared. Clear grid data before resize will be executed.";
    this->clear();
  }

  _data   = std::make_shared<std::vector<CellType>>(num_cells_x * num_cells_y);
  _cols   = num_cells_x;
  _rows   = num_cells_y;
  _stride = num_cells_x;
}

template <typename CellType>
bool GridDataMemoryHandler<CellType>::shareGridDataFrom(GridDataMemoryHandler& source, const base::Rect2u& roi)
{
  using LogError = base::Log<base::LogLevel::ERROR, base::LogGroup::ALGORITHM, class_name>;
  using LogDebug = base::Log<base::LogLevel::DEBUG, base::LogGroup::ALGORITHM, class_name>;

  if (roi.xMax() >= source._rows || roi.yMax() >= source._cols) {
    // Log error and clear this grid data to fall into a safe state.
    LogError() << "roi is out of range. source rows = " << source._rows << ", source cols = " << source._cols
               << ". roi = " << roi << ".";  
    this->clear();
    return false;
  }

  LogDebug() << "uses shared memory. Roi = " << roi << " from grid with following size: rows = " << source._rows
             << " cols = " << source._cols << ".";

  _is_shared_memory = true;

  _data   = source._data;
  _offset = roi.origin();
  _cols   = roi.width();
  _rows   = roi.height();
  _stride = source._cols;

  return true;
}





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

template <typename CellType>
Grid<CellType>& Grid<CellType>::operator=(Grid&& origin)
{
  // move content to this
  _data = std::move(origin._data);

  _cell_size   = origin._cell_size;
  _size_x      = origin._size_x;
  _size_y      = origin._size_y;

  // clear source object
  origin.clear();

  return *this;
}

template <typename CellType>
void Grid<CellType>::clear()
{
  _cell_size = 0.0;
  _size_x    = 0.0;
  _size_y    = 0.0;

  _data.clear();
}

} // end namespace mapping

} // end namespace francor