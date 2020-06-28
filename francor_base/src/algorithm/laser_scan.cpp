#include "francor_base/algorithm/laser_scan.h"
#include "francor_base/laser_scan.h"

namespace francor {

namespace base {

namespace algorithm {

namespace laser_scan {

Rect2d estimateRoi(const LaserScan& scan)
{
  const double range_diameter = scan.range() * 2.0;
  const double x_min = -scan.range();
  const double y_min = -scan.range();
  
  return { x_min, y_min, range_diameter, range_diameter };
}

} // end namespace laser_scan

} // end namespace algorithm

} // end namespace base

} // end namespace francor