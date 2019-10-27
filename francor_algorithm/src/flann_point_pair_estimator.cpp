/**
 * Flann point pair estimator implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#include "francor_algorithm/flann_point_pair_estimator.h"

#include <flann/flann.h>

#include <francor_base/log.h>

namespace francor {

namespace algorithm {

using francor::base::LogError;

FlannPointPairEstimator::~FlannPointPairEstimator()
{
  this->freeFlannIndex();
}

bool FlannPointPairEstimator::setPointDataset(const base::Point2dVector& points)
{
  this->copyPointDataset(points, _point_dataset);
  return this->createFlannIndex();
}

bool FlannPointPairEstimator::findPairs(const base::Point2dVector& points, PointPairIndexVector& pairs)
{
  if (_flann_index == nullptr)
  {
    LogError() << "FlannPointPairEstimator::findPairs(): no point dataset is set. Cancel pair estimating.";
    return false;
  }

  std::vector<float> target_dataset;
  this->copyPointDataset(points, target_dataset);

  std::vector<int> indicies(points.size());
  std::vector<float> distances(points.size());
  FLANNParameters parameter;

  // use automatic setup using the target precision  
  parameter.target_precision = 0.9;
  
  flann_find_nearest_neighbors_index(_flann_index, target_dataset.data(), points.size(), indicies.data(), distances.data(), 1, &parameter);
  this->copyIndexPairs(indicies, pairs);

  return true;
}

void FlannPointPairEstimator::copyPointDataset(const base::Point2dVector& points, std::vector<float>& dataset) const
{
  // allocate memory for dataset ((x,y) * num points)
  dataset.resize(points.size() * 2);

  for (std::size_t i = 0; i < points.size(); ++i) {
    dataset[i * 2 + 0] = points[i].x();
    dataset[i * 2 + 1] = points[i].y();
  }
}

void FlannPointPairEstimator::copyIndexPairs(const std::vector<int>& indices, PointPairIndexVector& pairs) const
{
  pairs.resize(indices.size());

  for (std::size_t i = 0; i < pairs.size(); ++i) {
    pairs[i].first = i;
    pairs[i].second = indices[i];
  }
}

bool FlannPointPairEstimator::createFlannIndex()
{
  // release previous index
  this->freeFlannIndex();

  // use automatic setup using the target precision
  FLANNParameters parameter;
  parameter.target_precision = 0.9;

  _flann_index = flann_build_index(_point_dataset.data(), _point_dataset.size() / 2, 2, nullptr, &parameter);

  if (_flann_index == nullptr) {
    LogError() << "FlannPointPairEstimator::createFlannIndex(): error occurred during flann index build.";
    return false;
  }
  
  return true;
}

void FlannPointPairEstimator::freeFlannIndex()
{
  if (_flann_index == nullptr)
    return;

  FLANNParameters parameter;
  flann_free_index(_flann_index, &parameter);
  _flann_index = nullptr;
}

} // end namespace algorithm

} // end namespace francor