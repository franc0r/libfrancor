/**
 * Algorithm and estimation function regarding line.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. October 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/point.h"
#include "francor_base/angle.h"

#include <cmath>

namespace francor {

namespace base {

namespace algorithm {

namespace line {

inline Vector2d calculateV(const Angle& phi)
{
  return { std::cos(phi),  std::sin(phi) };
}

inline Vector2d calculateV(const Point2d& p0, const Point2d& p1)
{
  return (p1 - p0).normalized();
}

} // end namespace line

} // end namespace algorithm

} // end namespace base

} // end namespace francor