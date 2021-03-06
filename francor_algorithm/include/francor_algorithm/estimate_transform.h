/**
 * Collection of function to estimate transform between data sets.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include <francor_base/transform.h>
#include <francor_base/point.h>

#include "francor_algorithm/point_pair_estimator.h"

namespace francor {

namespace algorithm {

/**
 * \brief Estimates the transformation between the two given 2d point datasets.
 * 
 * \param dataset_a Dataset A.
 * \param dataset_b Dataset B.
 * \param pair_indices Valid paris of both datasets.
 * \param max_distance Maximal distance between a point pair. If the distance is greather the pair will be ignored.
 * \param transform Transformation between the two given datasets (from a to b) will be set.
 * \return rms value of estimation of < 0 in case of an error.
 */
double estimateTransform(const base::Point2dVector& dataset_a,
                         const base::Point2dVector& dataset_b,
                         const PointPairIndexVector& pair_indices,
                         const double max_distance,
                         base::Transform2d& transform);

} // end namespace algorithm

} // end namespace francor