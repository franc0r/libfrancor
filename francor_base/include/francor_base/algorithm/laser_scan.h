/**
 * Algorithm for laser scan sensor measurements.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 28. June 2020
 */
#pragma once

#include "francor_base/rect.h"

namespace francor {

namespace base {

class LaserScan;

namespace algorithm {

namespace laser_scan {

/**
 * \brief Estimates a rectangle according FOV of laser scan representing a area the was observed by the sensor.
 * 
 * \param scan Laser scan of the sensor.
 * \return Axis aligned rectangle representing FOV of the sensor.
 */
Rect2d estimateRoi(const LaserScan& scan);

} // end namespace laser_scan

} // end namespace algorithm

} // end namespace base

} // end namespace francor