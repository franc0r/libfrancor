/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include <Eigen/Dense>

#include <vector>

namespace francor
{

namespace base
{

using Vector2d = Eigen::Vector2d;
using Vector3d = Eigen::Vector3d;

using VectorVector2d = std::vector<Vector2d, Eigen::aligned_allocator<Vector2d>>;
using VectorVector3d = std::vector<Vector2d, Eigen::aligned_allocator<Vector3d>>;

} // end namespace base

} // end namespace francor