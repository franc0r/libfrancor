/**
 * Algorithm and estimation function regarding line.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */

#include "francor_base/algorithm/point.h"
#include "francor_base/laser_scan.h"

namespace francor {

namespace base {

namespace algorithm {

namespace point {

bool convertLaserScanToPoints(const LaserScan& scan, Point2dVector& points)
{
  points.resize(0);
  points.reserve(scan.distances().size());
  Angle current_phi = scan.phiMin();

  for (const auto distance : scan.distances()) {
    if (std::isnan(distance) || std::isinf(distance))
      continue;

    points.push_back( { distance * std::cos(current_phi), distance * std::sin(current_phi) } );
  }

  return true;
}

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor