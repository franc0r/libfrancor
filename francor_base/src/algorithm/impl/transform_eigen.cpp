#include "francor_base/algorithm/impl/transform_eigen.h"
#include "francor_base/transform.h"

namespace francor {

namespace base {

namespace algorithm {

namespace transform {

namespace eigen {

void transformPointVector(const Transform2d& transform, Point2dVector& points)
{
  for (auto& point : points)
    point = transform * point;
}

} // end namespace eigen

} // end namespace transform

} // end namespace algorithm

} // end namespace base

} // end namespace francor