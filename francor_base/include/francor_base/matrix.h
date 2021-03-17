/**
 * Defines matrix classes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 25. April 2019
 */
#pragma once

#include <Eigen/Dense>

namespace francor
{

namespace base
{

using Matrix2d = Eigen::Matrix2d;
using Matrix3d = Eigen::Matrix3d;
using Matrix4d = Eigen::Matrix4d;

template <typename Type, std::size_t Rows, std::size_t Cols>
using Matrix = Eigen::Matrix<Type, Rows, Cols>;

} // end namespace base

} // end namespace francor