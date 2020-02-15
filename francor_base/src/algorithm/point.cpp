/**
 * Algorithm and estimation function regarding line.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */

#include "francor_base/algorithm/point.h"
#include "francor_base/laser_scan.h"
#include "francor_base/transform.h"
#include "francor_base/log.h"

#include <iostream>

namespace francor {

namespace base {

namespace algorithm {

namespace point {

bool convertLaserScanToPoints(const LaserScan& scan, const Pose2d& ego_pose, Point2dVector& points)
{
  points.resize(0);
  points.reserve(scan.distances().size());
  const Transform2d transform( { ego_pose.orientation() }, { ego_pose.position().x(), ego_pose.position().y() } );
  const Pose2d start(transform * scan.pose());
  Angle current_phi = scan.phiMin() + start.orientation();
  
  std::cout << "start " << start << std::endl;
  std::cout << "laser scan = " << scan << std::endl;

  for (const auto distance : scan.distances())
  {
    if (!(std::isnan(distance) || std::isinf(distance))) {
      const Point2d point(distance * std::cos(current_phi), distance * std::sin(current_phi));
      points.push_back(point + start.position());
    }

    current_phi += scan.phiStep();
  }

  return true;
}

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor