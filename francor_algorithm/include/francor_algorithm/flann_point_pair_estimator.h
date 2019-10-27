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

  bool setPointDataset(const base::Point2dVector& points) final;
  bool findPairs(const base::Point2dVector& points, PointPairIndexVector& pairs) final;

private:
  void copyPointDataset(const base::Point2dVector& points, std::vector<float>& dataset) const;
  void copyIndexPairs(const std::vector<int>& indices, PointPairIndexVector& pairs) const;
  bool createFlannIndex();
  void freeFlannIndex();

  std::vector<float> _point_dataset;
  void* _flann_index = nullptr;
};

} // end namespace algorithm

} // end namespace francor