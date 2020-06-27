/**
 * \brief Class that represents a 2d tsd grid. Note: POC
 */
#pragma once

#include "francor_mapping/grid.h"

#include <ostream>

namespace francor {

namespace mapping {

struct TsdCell
{
  double tsd    = 0.0;
  double weight = 0.0;
};

class TsdGrid : public Grid<TsdCell>
{
public:
  TsdGrid();

  inline void setMaxTruncation(const double max_truncation) noexcept { _max_truncation = max_truncation; }
  inline double getMaxTruncation() const noexcept { return _max_truncation; }
  
private:
  double _max_truncation = 100.0;
};

} // end namespace mapping

} // end namespace francor


namespace std {

inline ostream& operator<<(ostream& os, const francor::mapping::TsdCell& cell)
{
  os << "(" << cell.tsd << ", " << cell.weight << ")";
  return os;
}

inline ostream& operator<<(ostream& os, const francor::mapping::TsdGrid& grid)
{
  os << "tsd grid:" << std::endl;
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