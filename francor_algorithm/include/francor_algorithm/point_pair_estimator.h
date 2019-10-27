/**
 * Point pair estimator interface definition.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include <francor_base/point.h>

namespace francor {

namespace algorithm {

using PointPairIndex = std::pair<std::size_t, std::size_t>;
using PointPairIndexVector = std::vector<PointPairIndex>;

class PointPairEstimator
{
protected:
  PointPairEstimator() = default;

public:
  virtual ~PointPairEstimator() = default;

  virtual bool setPointDataset(const base::Point2dVector& points) = 0;
  virtual bool findPairs(const base::Point2dVector& points, PointPairIndexVector& pairs) = 0;
};

} // end namespace algorithm

} // end namespace francor