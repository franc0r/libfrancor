/**
 * Flann point pair estimator implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#include "francor_algorithm/flann_point_pair_estimator.h"

#include <flann/flann.h>

namespace francor {

namespace algorithm {

FlannPointPairEstimator::~FlannPointPairEstimator()
{

}

void FlannPointPairEstimator::setPointDataset(const base::Point2dVector& points)
{

}

PointPairIndexVector FlannPointPairEstimator::findPairs(const base::Point2dVector& points)
{

}

void FlannPointPairEstimator::copyPointDataset(const base::Point2dVector& points)
{

}

void FlannPointPairEstimator::createFlannIndex()
{

}

} // end namespace algorithm

} // end namespace francor