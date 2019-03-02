/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. March 2019
 */
#pragma once

#include "francor_base/line.h"

#include <iostream>

namespace francor {

namespace algorithm {

Line fittingLineFromPoints(const VectorVector2d& points);

} // end namespace algorithm

} // end namespace francor