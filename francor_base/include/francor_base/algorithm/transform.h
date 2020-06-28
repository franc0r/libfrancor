/**
 * Algorithm and estimation function regarding transform.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include "francor_base/point.h"
#include "francor_base/algotihm/impl/transform_std.h"

namespace francor {

namespace base {

class Transform2d;

namespace algorithm {

namespace transform {

using francor::base::algorithm::transform::std::transformPointVector;

} // end namespace transform

} // end namespace algorithm

} // end namespace base

} // end namespace francor