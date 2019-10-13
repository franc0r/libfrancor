/**
 * Algorithm and estimation function regarding tsd.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include "francor_mapping/tsd_grid.h"

namespace francor {

namespace base {
class LaserScan;
}

namespace mapping {

namespace algorithm {

namespace tsd {


void pushLaserScanToGrid(TsdGrid& grid, const base::LaserScan& scan);


} // end namespace tsd

} // end namespace algorithm

} // end namespace mapping

} // end namespace francor