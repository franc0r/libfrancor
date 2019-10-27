/**
 * Flann point pair estimator implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#include "francor_algorithm/flann_point_pair_estimator.h"

#include <flann/flann.hpp>

#include <francor_base/log.h>

namespace francor {

namespace algorithm {

using francor::base::LogError;

FlannPointPairEstimator::FlannPointPairEstimator()
{

}

FlannPointPairEstimator::~FlannPointPairEstimator()
{

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

  std::vector<double> target_dataset;
  this->copyPointDataset(points, target_dataset);

  _indicies.resize(points.size());
  _distances.resize(points.size());

  flann::Matrix<int> indices(_indicies.data(), _indicies.size(), 1);
  flann::Matrix<double> distances(_distances.data(), _distances.size(), 1);
  flann::Matrix<double> target(target_dataset.data(), _indicies.size(), 2);
  flann::SearchParams parameter(-1, 1e-2);

  _flann_index->knnSearch(target, indices, distances, 1, parameter);

  this->copyIndexPairs(_indicies, pairs);

  return true;
}

void FlannPointPairEstimator::copyPointDataset(const base::Point2dVector& points, std::vector<double>& dataset) const
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
    pairs[i].first = indices[i];
    pairs[i].second = i;
  }
}

bool FlannPointPairEstimator::createFlannIndex()
{
  // \todo select good parameter
  flann::KDTreeSingleIndexParams parameter;
  const flann::Matrix<double> points(_point_dataset.data(), _point_dataset.size() / 2, 2);

  _flann_index = std::make_unique<flann::Index<flann::L2<double>>>(points, parameter); 
  _flann_index->buildIndex();
  
  return true;
}

} // end namespace algorithm

} // end namespace francor