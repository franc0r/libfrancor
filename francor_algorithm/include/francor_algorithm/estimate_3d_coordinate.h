/**
 * Estimates a 3d coordinate.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 25. April 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/matrix.h"

namespace francor
{

namespace algorithm
{

bool estimate3dCoordinateOnGroundplane(const base::Matrix3d& intrinsic, const base::Matrix4d& extrinsic, const base::Vector2d& input, base::Vector3d& output)
{

}

} // end namespace algorithm

} // end namespace francor