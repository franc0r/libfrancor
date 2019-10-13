/**
 * Represents a ego object like the robot.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include <francor_base/pose.h>

namespace francor {

namespace mapping {

class EgoObject
{
public:
  EgoObject(const base::Pose2d& pose)
    : _pose(pose) { }

  inline constexpr const base::Pose2d& pose() const noexcept { return _pose; }
  inline constexpr void setPose(const base::Pose2d& pose) noexcept { _pose = pose; }

private:
  base::Pose2d _pose;
  // \todo add covariances
};

} // end namespace mapping

} // end namespace francor