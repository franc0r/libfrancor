/**
 * Contains geometry math functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. March 2019
 */
#pragma once

#include "francor_base/line.h"

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::VectorVector2d& points);

} // end namespace algorithm

} // end namespace francor