/**
 * \brief This abstract base class that represents a 2d grid. Note: POC
 */
#pragma once

#include <memory>
#include <limits>
#include <ostream>

#include <francor_base/log.h>
#include <francor_base/point.h>
#include <francor_base/rect.h>

namespace francor {

namespace mapping {

template <typename CellType>
class GridDataMemoryHandler
{
public:
  GridDataMemoryHandler(const std::size_t numCellsX = 0, const std::size_t numCellsY = 0)
  {
    this->resize(numCellsX, numCellsY);
  }
  GridDataMemoryHandler(GridDataMemoryHandler& handler, const base::Rect2u& roi)
  {
    this->clear();
    this->shareGridDataFrom(handler, roi);
  }
  GridDataMemoryHandler(GridDataMemoryHandler&& origin)
  {
    *this = std::move(origin);
  }
  GridDataMemoryHandler(const GridDataMemoryHandler&) = default;

  GridDataMemoryHandler& operator=(const GridDataMemoryHandler&) = default;
  GridDataMemoryHandler& operator=(GridDataMemoryHandler&& origin)
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

  inline constexpr std::size_t cols() const noexcept { return _cols; }
  inline constexpr std::size_t rows() const noexcept { return _rows; }
  inline constexpr bool isInitialized() const noexcept { return _data != nullptr; }

  inline CellType& operator()(const std::size_t x, const std::size_t y)
  {
    return (*_data)[(y + _offset.y()) * _stride + x + _offset.y()];
  }
  inline const CellType& operator()(const std::size_t x, const std::size_t y) const
  {
    return (*_data)[(y + _offset.y()) * _stride + x + _offset.y()];
  }
  
  void clear()
  {
    _is_shared_memory = false;
    
    _data   = nullptr;
    _offset = {0, 0};
    _cols   = 0;
    _rows   = 0;
    _stride = 0;
  }

  void resize(const std::size_t num_cells_x, const std::size_t num_cells_y)
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

private:
  bool shareGridDataFrom(GridDataMemoryHandler& source, const base::Rect2u& roi)
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

  bool _is_shared_memory = false;
  std::shared_ptr<std::vector<CellType>> _data;
  base::Point2u _offset{0, 0};
  std::size_t _cols   = 0;
  std::size_t _rows   = 0;     //> number of cells in x dimension
  std::size_t _stride = 0;     //> number of cells in y dimension 

public:
  static constexpr const char class_name[] = "GridDataMemoryHandler";
};

template <typename CellType>
class Grid
{
public:
  /**
   * \brief Default constructor. Constructs an empty invalid grid.
   */
  Grid() = default;
  Grid(const Grid&) = default;
  /**
   * \brief Moves all context to this and resets the origin grid object.
   */
  Grid(Grid&& origin);
  Grid(Grid& origin, const base::Rect2u& roi);
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

    _cell_size   = origin._cell_size;
    _size_x      = origin._size_x;
    _size_y      = origin._size_y;

    origin.clear();

    return *this;
  }

  /**
   * \brief Initialize this grid using the given arguments. The size is given in number of cells in x and y direction.
   * 
   * \param munCellsX number of cells in x direction.
   * \param numCellsY number of cells in y direction.
   * \param cellSize size (edge length) of each cell in meter.
   * \return true if grid was successfully initialized.
   */
  bool init(const std::size_t numCellsX, const std::size_t numCellsY, const double cellSize);


  /**
   * \brief Resets this grid. The grid will be empty and invalid.
   */
  void clear()
  {
    _cell_size   = 0.0;
    _size_x      = 0.0;
    _size_y      = 0.0;

    _data.clear();
  }

  /**
   * \brief Checks if this grid is empty.
   * 
   * \return true if grid is empty.
   */
  bool isEmpty() const { return _data.rows() == 0 && _data.cols() == 0; }
  /**
   * \brief Checks if this grid is valid. The cell size must be greater than zero and the grid size min 1x1.
   * 
   * \return true if grid is valid.
   */
  bool isValid() const { return _cell_size > 0.0 && _data.cols() > 0 && _data.rows() > 0; }
  /**
   * \brief Returns the current number of cells in x direction.
   * 
   * \return number of cells in x direction.
   */
  inline std::size_t getNumCellsX() const noexcept { return _data.cols(); }
  /**
   * \brief Returns the current number of cells in y direction.
   * 
   * \return number of cells in y direction.
   */
  inline std::size_t getNumCellsY() const noexcept { return _data.rows(); }
  /**
   * \brief Returns the cell size in meter. The size represents both edge lengths of a cell.
   * 
   * \return the cell size in meter.
   */
  inline double getCellSize() const noexcept { return _cell_size; }
  /**
   * \brief Returns the size in x direction of this grid in meters.
   * 
   * \return size x in meter.
   */
  inline double getSizeX() const noexcept { return _size_x; }
  /**
   * \brief Returns the size in y direction of this grid in meters.
   * 
   * \return size y in meter.
   */
  inline double getSizeY() const noexcept { return _size_y; }
  /**
   * \brief Operator to access the cell at the given coordinates.
   * 
   * \return reference to the cell.
   */
  inline CellType& operator()(const std::size_t x, const std::size_t y) { return _data(x, y); }
  inline const CellType& operator()(const std::size_t x, const std::size_t y) const { return _data(x, y); }
  /**
   * \brief Estimates the index to the given x coordinate.
   * 
   * \param x x-cooridnate
   * \return x index for given x-coordinate
   */
  inline std::size_t getIndexX(const double x) const { return static_cast<std::size_t>((x + _origin.x()) / _cell_size); }
  /**
   * \brief Estimates the index to the given y coordinate.
   * 
   * \param y y-cooridnate
   * \return y index for given y-coordinate
   */
  inline std::size_t getIndexY(const double y) const { return static_cast<std::size_t>((y + _origin.y()) / _cell_size); }
  /**
   * \brief Returns the position of the selected cell.
   * 
   * \param x The x index of the wanted cell.
   * \param y The y index of the wanted cell.
   * \return the position (mid) of the selected cell in meter.
   */
  inline base::Point2d getCellPosition(const std::size_t x, const std::size_t y) const
  {
    return { x * _cell_size + 0.5 * _cell_size, y * _cell_size + 0.5 * _cell_size };
  }
  /**
   * \brief Returns the origin of this grid. The origin is located at cell (0, 0).
   * \return Origin coordinate in meter.
   */
  inline const base::Point2d& getOrigin() const noexcept { return _origin; }

private:
  double _cell_size = 0.0; //> size of each cell
  double _size_x    = 0.0; //> size in m in x dimension
  double _size_y    = 0.0; //> size in m in y dimension

  base::Point2d _origin{0.0, 0.0}; //> origin coordinate of this grid in meter

  GridDataMemoryHandler<CellType> _data; //> grid cells

public:
  static constexpr const char class_name[] = "Grid<CellType>";
};

} // end namespace mapping

} // end namespace francor


namespace std {

template <typename CellType>
inline ostream& operator<<(ostream& os, const francor::mapping::Grid<CellType>& grid)
{
  os << "occupancy grid:" << std::endl;
  os << "num cells x = " << grid.getNumCellsX() << std::endl;
  os << "num cells y = " << grid.getNumCellsY() << std::endl;
  os << "cell size = " << grid.getCellSize() << " m" << std::endl;
  os << "data[]:" << std::endl;

  for (std::size_t row = 0; row < grid.getNumCellsY(); ++row)
  {
    for (std::size_t col = 0; col < grid.getNumCellsX(); ++col)
      os << grid(col, row) << " ";

    os << std::endl;
  }

  return os;
}

} // end namespace std