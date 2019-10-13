/**
 * Laser scan class that represents a scan from a 2d Lidar.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 12. October 2019
 */
#pragma once

#include <vector>

#include "francor_base/angle.h"

namespace francor {

namespace base {

class LaserScan
{
public:
  LaserScan() = default;
  LaserScan(const std::vector<double>& distances,
            const Angle phiMin,
            const Angle phiMax,
            const Angle phiStep,
            const Angle divergence = 0.0)
    : _distances(distances),
      _phi_step(phiStep),
      _phi_min(phiMin),
      _phi_max(phiMax),
      _divergence(divergence)
  { }

  inline Angle phiMax() const noexcept { return _phi_max; }
  inline Angle phiMin() const noexcept { return _phi_min; }
  inline Angle phiStep() const noexcept { return _phi_step; }
  inline Angle divergence() const noexcept { return _divergence; }
  inline const std::vector<double> distances() const noexcept { return _distances; }

private:
  std::vector<double> _distances;
  Angle _phi_step{0.0};
  Angle _phi_min{0.0};
  Angle _phi_max{0.0};
  Angle _divergence{0.0};
};

} // end namespace base

} // end namespace francor