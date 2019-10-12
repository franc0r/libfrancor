/**
 * Contains geometry math functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. March 2019
 */
#pragma once

#include "francor_base/line.h"
#include "francor_base/line_segment.h"

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::Point2dVector& points,
                                 const std::vector<std::size_t>& indices = std::vector<std::size_t>());

base::LineSegment fittingLineSegmentFromPoints(const base::Point2dVector& points,
                                               const std::vector<std::size_t>& indices = std::vector<std::size_t>());                                 

} // end namespace algorithm

} // end namespace francor