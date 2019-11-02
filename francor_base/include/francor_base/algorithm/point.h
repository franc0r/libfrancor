/**
 * Algorithm and estimation function regarding line.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include "francor_base/point.h"

namespace francor {

namespace base {

class LaserScan;

namespace algorithm {

namespace point {

/**
 * \brief Converts an laser scan to 2d point vector. Nan and inf distance value of the scan will be ignored.
 * 
 * \param scan Laser scan that will be converted.
 * \param points Resulting 2d points stored in a vector.
 * \return true if convertion was successful.
 */
bool convertLaserScanToPoints(const LaserScan& scan, Point2dVector& points);

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor