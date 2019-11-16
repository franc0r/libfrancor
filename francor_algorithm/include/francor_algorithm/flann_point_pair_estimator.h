/**
 * Flann point pair estimator implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include <memory>

#include "francor_algorithm/point_pair_estimator.h"

// forward declatration for flann index pointer
namespace flann {
template <typename T>
class Index;

template <typename T>
class L2;

template <typename T>
class Matrix;

} // end namespace flann

namespace francor {

namespace algorithm {

class FlannPointPairEstimator final : public PointPairEstimator
{
public:
  FlannPointPairEstimator();
  ~FlannPointPairEstimator() final;

  bool setPointDataset(const base::Point2dVector& points) final;
  bool findPairs(const base::Point2dVector& points, PointPairIndexVector& pairs) final;

private:
  void copyPointDataset(const base::Point2dVector& points, std::vector<double>& dataset) const;
  void copyIndexPairs(const std::vector<int>& indices,
                      const std::vector<double>& distances,
                      PointPairIndexVector& pairs) const;
  bool createFlannIndex();

  std::unique_ptr<flann::Index<flann::L2<double>>> _flann_index;
  std::vector<double> _point_dataset;
  std::vector<int> _indicies;
  std::vector<double> _distances;  
};

} // end namespace algorithm

} // end namespace francor