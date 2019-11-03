/**
 * \brief This abstract base class that represents a 2d grid. Note: POC
 */
#pragma once

#include <limits>

#include <francor_base/log.h>
#include <francor_base/point.h>

namespace francor {

namespace mapping {

template<typename CellType>
class Grid
{
public:
  /**
   * \brief Default constructor. Constructs an empty invalid grid.
   */
  Grid(void) = default;
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
  virtual ~Grid(void) = default;
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

    _num_cells_x = origin._num_cells_x;
    _num_cells_y = origin._num_cells_y;
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
  bool init(const std::size_t numCellsX, const std::size_t numCellsY, const double cellSize)
  {
    if (cellSize <= std::numeric_limits<double>::min())
    {
      base::LogError() << "Grid: cell size must be greater than zero. Can't initialize grid. cell size = " << cellSize;
      return false;
    }

    // allocate grid data
    _data.resize(numCellsX * numCellsY);

    _num_cells_x = numCellsX;
    _num_cells_y = numCellsY;
    _cell_size = cellSize;

    // calculate size from other parameter
    _size_x = _num_cells_x * _cell_size;
    _size_y = _num_cells_y * _cell_size;

    return true;
  }

  /**
   * \brief Initialize this grid using the given arguments. The size is given in meters in x and y direction.
   * 
   * \param sizeX length of the grid in x direction. The length is adjusted depending an the cell size, so it fits
   *              to fully cells.
   * \param sizeY length of the grid in y direction. The length is adjusted depending an the cell size, so it fits
   *              to fully cells.
   * \param cellSize size (edge length) of each cell in meter. 
   * \return true if grid was successfully initialized.
   */
  //
  // TODO: is ambiguous. Check if this method is really necessary.
  //
  // bool init(const double sizeX, const double sizeY, const double cellSize)
  // {
  //   if (cellSize <= std::numeric_limits<double>::min())
  //   {
  //     base::LogError() << "Grid: cell size must be greater than zero. Can't initialize grid. cell size = " << cellSize;
  //     return false;
  //   }
  //   if (sizeX <= std::numeric_limits<double>::min())
  //   {
  //     base::LogError() << "Grid: size x must be greater than zero. Can't initialize grid. size x = " << sizeX;
  //     return false;
  //   }
  //   if (sizeY <= std::numeric_limits<double>::min())
  //   {
  //     base::LogError() << "Grid: size y must be greater than zero. Can't initialize grid. size y = " << sizeY;
  //     return false;
  //   }

  //   const auto numCellsX = static_cast<std::size_t>(sizeX / cellSize);
  //   const auto numCellsY = static_cast<std::size_t>(sizeY / cellSize);

  //   return this->init(numCellsX, numCellsY, cellSize);
  // }

  /**
   * \brief Resets this grid. The grid will be empty and invalid.
   */
  void clear(void)
  {
    _num_cells_x = 0;
    _num_cells_y = 0;
    _cell_size = 0.0;
    _size_x = 0.0;
    _size_y = 0.0;

    _data.clear();
  }

  /**
   * \brief Checks if this grid is empty.
   * 
   * \return true if grid is empty.
   */
  bool isEmpty() const { return _num_cells_x == 0 && _num_cells_y == 0; }
  /**
   * \brief Checks if this grid is valid. The cell size must be greater than zero and the grid size min 1x1.
   * 
   * \return true if grid is valid.
   */
  bool isValid() const { return _cell_size > 0.0 && _num_cells_x > 0 && _num_cells_y > 0; }
  /**
   * \brief Returns the current number of cells in x direction.
   * 
   * \return number of cells in x direction.
   */
  inline std::size_t getNumCellsX(void) const noexcept { return _num_cells_x; }
  /**
   * \brief Returns the current number of cells in y direction.
   * 
   * \return number of cells in y direction.
   */
  inline std::size_t getNumCellsY(void) const noexcept { return _num_cells_y; }
  /**
   * \brief Returns the cell size in meter. The size represents both edge lengths of a cell.
   * 
   * \return the cell size in meter.
   */
  inline double getCellSize(void) const noexcept { return _cell_size; }
  /**
   * \brief Returns the size in x direction of this grid in meters.
   * 
   * \return size x in meter.
   */
  inline double getSizeX(void) const noexcept { return _size_x; }
  /**
   * \brief Returns the size in y direction of this grid in meters.
   * 
   * \return size y in meter.
   */
  inline double getSizeY(void) const noexcept { return _size_y; }
  /**
   * \brief Operator to access the cell at the given coordinates.
   * 
   * \return reference to the cell.
   */
  inline CellType& operator()(const std::size_t x, const std::size_t y) { return _data[y * _num_cells_x + x]; }
  inline const CellType& operator()(const std::size_t x, const std::size_t y) const { return _data[y * _num_cells_x + x]; }
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
  std::size_t _num_cells_x = 0;    //> number of cells in x dimension
  std::size_t _num_cells_y = 0;    //> number of cells in y dimension
  double _cell_size = 0.0;         //> size of each cell
  double _size_x = 0.0;            //> size in m in x dimension
  double _size_y = 0.0;            //> size in m in y dimension
  base::Point2d _origin{0.0, 0.0}; //> origin coordinate of this grid in meter

  std::vector<CellType> _data;     //> grid cells
};

} // end namespace mapping

} // end namespace francor