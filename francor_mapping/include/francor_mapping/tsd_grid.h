/**
 * \brief Class that represents a 2d tsd grid. Note: POC
 */
#pragma once

#include "francor_mapping/grid.h"

namespace francor {

namespace mapping {

struct TsdCell
{
  double tsd    = 0.0;
  double weight = 0.0;
};

using TsdGrid = Grid<TsdCell>;

} // end namespace mapping

} // end namespace francor