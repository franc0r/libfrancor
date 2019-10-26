/**
 * Flann point pair estimator implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include "francor_algorithm/point_pair_estimator.h"

namespace francor {

namespace algorithm {

class FlannPointPairEstimator final : public PointPairEstimator
{
public:
  FlannPointPairEstimator() = default;
  ~FlannPointPairEstimator() final;

  void setPointDataset(const base::Point2dVector& points) final;
  PointPairIndexVector findPairs(const base::Point2dVector& points) final;

private:
  void copyPointDataset(const base::Point2dVector& points);
  void createFlannIndex();

  float* _point_dataset = nullptr;
  int _flann_index = -1;
};

} // end namespace algorithm

} // end namespace francor