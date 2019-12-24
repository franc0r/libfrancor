/**
 * \brief Occupancy grid class. Each cell contains a propability of an occupancy (range: 0 .. 100 -> 0% .. 100%).
 */
#pragma once

#include "francor_mapping/grid.h"

#include <iomanip>

namespace francor {

namespace mapping {

struct OccupancyCell
{
  float value = std::numeric_limits<float>::quiet_NaN();
};

class OccupancyGrid : public Grid<OccupancyCell>
{
public:
  OccupancyGrid() = default;
};

} // end namespace mapping

} // end namespace francor


namespace std {

inline ostream& operator<<(ostream& os, const francor::mapping::OccupancyCell& cell)
{
  os << "(" << setw(3);
  if (std::isnan(cell.value)) {
    os << "nan";
  }
  else {
    os << cell.value;
  }
  os << ")";

  return os;
}

inline ostream& operator<<(ostream& os, const francor::mapping::OccupancyGrid& grid)
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