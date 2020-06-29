/**
 * Algorithm and estimation function regarding transform.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 28. June 2020
 */
#pragma once

#include "francor_base/point.h"

namespace francor {

namespace base {

class Transform2d;

namespace algorithm {

namespace transform {

namespace eigen {

/**
 * \brief Transforms a set of points by given transform.
 * 
 * \param transform The points will be transformed by that transfrom.
 * \param points Transformed points.
 */
void transformPointVector(const Transform2d& transform, Point2dVector& points);

} // end namespace eigen

} // end namespace transform

} // end namespace algorithm

} // end namespace base

} // end namespace francor