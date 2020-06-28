/**
 * Algorithm and estimation function regarding transform.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */

#include "francor_base/algorithm/impl/transform_std.h"
#include "francor_base/transform.h"

namespace francor {

namespace base {

namespace algorithm {

namespace transform {

namespace std {

void transformPointVector(const Transform2d& transform, Point2dVector& points)
{
  for (auto& point : points)
    point = transform * point;
}

} // end namespace std

} // end namespace transform

} // end namespace algorithm

} // end namespace base

} // end namespace francor