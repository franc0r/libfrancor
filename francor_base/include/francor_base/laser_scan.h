/**
 * Laser scan class that represents a scan from a 2d Lidar.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 12. October 2019
 */
#pragma once

#include <vector>
#include <ostream>

#include "francor_base/angle.h"
#include "francor_base/pose.h"

namespace francor {

namespace base {

class LaserScan
{
public:
  LaserScan() = default;
  LaserScan(const std::vector<double>& distances,
            const Pose2d& pose,
            const Angle phiMin,
            const Angle phiMax,
            const Angle phiStep,
            const double range,
            const Angle divergence = 0.0)
    : _distances(distances),
      _pose(pose),
      _phi_step(phiStep),
      _phi_min(phiMin),
      _phi_max(phiMax),
      _range(range),
      _divergence(divergence)
  { }

  inline Angle phiMax() const noexcept { return _phi_max; }
  inline Angle phiMin() const noexcept { return _phi_min; }
  inline Angle phiStep() const noexcept { return _phi_step; }
  inline Angle divergence() const noexcept { return _divergence; }
  inline const std::vector<double> distances() const noexcept { return _distances; }
  inline const Pose2d& pose() const noexcept { return _pose; }
  inline double range() const noexcept { return _range; }

private:
  std::vector<double> _distances;
  Pose2d _pose;
  Angle _phi_step{0.0};
  Angle _phi_min{0.0};
  Angle _phi_max{0.0};
  double _range{0.0};
  Angle _divergence{0.0};
};

} // end namespace base

} // end namespace francor


namespace std {

inline ostream& operator<<(ostream& os, const francor::base::LaserScan& scan)
{
  os << "### laser scan ###" << std::endl;
  os << "pose       : " << scan.pose() << std::endl;
  os << "phi min    : " << scan.phiMin() << std::endl;
  os << "phi max    : " << scan.phiMax() << std::endl;
  os << "phi step   : " << scan.phiStep() << std::endl;
  os << "range      : " << scan.range() << std::endl;
  os << "distances[]: ";
  
  for (const auto& distance : scan.distances())
    os << "[" << distance << "] ";

  return os;
}

} // end namespace std