/**
 * Contains geometry math functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. March 2019
 */
#pragma once

#include "francor_base/line.h"
#include "francor_base/line_segment.h"

#include <optional>

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::Point2dVector& points,
                                 const std::vector<std::size_t>& indices = std::vector<std::size_t>());

base::LineSegment fittingLineSegmentFromPoints(const base::Point2dVector& points,
                                               const std::vector<std::size_t>& indices = std::vector<std::size_t>());                                 

/**
 * \brief Estimates normals from sorted point vector.
 * 
 * \param points Input points. The normals will be estimated from that points. The points must be ordered.
 * \param n Uses n points for normal estimation. The number of points must be odd.
 * \return the resulting normal stored in a vector if it was successfull otherwise false.
 */
std::optional<std::vector<base::AnglePiToPi>> estimateNormalsFromOrderedPoints(const base::Point2dVector& points, const int n = 3);

} // end namespace algorithm

} // end namespace francor