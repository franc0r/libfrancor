/**
 * Pose classes
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include "francor_base/point.h"
#include "francor_base/angle.h"

#include <ostream>

namespace francor {

namespace base {

class Pose2d
{
public:
  constexpr Pose2d() : _position(0.0, 0.0), _orientation(0.0) { }
  constexpr Pose2d(const Point2d& pos, const Angle angle) : _position(pos), _orientation(angle) { }

  inline constexpr const Point2d& position() const noexcept { return _position; }
  inline constexpr void setPosition(const Point2d& pos) noexcept { _position = pos; }
  inline constexpr Angle orientation() const noexcept { return _orientation; }
  inline constexpr void setOrientation(const Angle angle) noexcept { _orientation = angle; }

private:
  Point2d _position;
  Angle _orientation;
};

} // end namespace base

} // end namespace francor


namespace std {

inline ostream& operator<<(ostream& os, const francor::base::Pose2d& pose)
{
  os << "pose: [" << pose.orientation() << ", " << pose.position() << "]";
  return os;
}

} // end namespace std