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

/**
 * \brief Estimates the postion of an pixel on the ground plane based on the intrinsic camera parameters and
 *        the pose of the camera (extrinsic).
 * 
 * \param intrinsic The intrinsic camera parameters [ [ fx 0 cx ], [ 0 fy cy ] [ 0 0 1 ] ].
 * \param extrinsic The camera pose as homogene transformation matrix.
 * \param input The 2d pixel coordinate.
 * \param output The estimated 3d coordinate.
 * \return true if estimation was successfully.
 */
bool estimate3dCoordinateOnGroundplane(const base::Matrix3d& intrinsic,
                                       const base::Matrix4d& extrinsic,
                                       const base::Vector2d& input,
                                       base::Vector3d& output)
{

}

} // end namespace algorithm

} // end namespace francor