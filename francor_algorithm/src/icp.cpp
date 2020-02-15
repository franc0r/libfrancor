/**
 * ICP implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. October 2019
 */

#include "francor_algorithm/icp.h"

#include <francor_base/log.h>

namespace francor {

namespace algorithm {

using francor::base::LogError;
using francor::base::LogWarn;
using francor::base::LogDebug;

bool Icp::estimateTransform(const base::Point2dVector& origin, const base::Point2dVector& target, base::Transform2d& transform) const
{
  if (_pair_estimator == nullptr) {
    LogError() << "Icp::estimateTransform(): no point pair estimator is set. Cancel estimation.";
    return false;
  }

  // set origin set as model (reference)
  if (!_pair_estimator->setPointDataset(origin)) {
    LogError() << "Icp::estimateTransform(): error occurred during setting point dataset. Cancel estimation process.";
    return false;
  }

  // get a copy the target points
  base::Point2dVector moved_points(target);
  double rms = _max_rms;
  transform.setRotation(0.0);
  transform.setTranslation({ 0.0, 0.0 });

  // iterate until max iterations is reached or a other criterion is fulfilled
  for (std::size_t iteration = 0; iteration < _max_iterations; ++iteration)
  {
    base::Transform2d current_transform;
    double current_rms;

    // do iteration and estimate transformation
    if (!this->doIteration(origin, moved_points, current_transform, rms * 10.0, current_rms)) {
      return false;
    }

    // check if current estimation rms is bigger than previous one 
    // it seems not a good idea to cancel the estimation using this criterion
    // if (current_rms >= rms) {
    //   return true;
    // }

    // integrate current iteration results
    rms = current_rms;
    // \todo I expect the opposite 
    transform = transform * current_transform;

    // check if estimated transformation is good enough
    if (current_rms <= _termination_rms) {
      return true;
    }

    // transform points
    current_transform = current_transform.inverse();
    
    for (auto& point : moved_points)
      point = current_transform * point;
  }

  return true;
}

bool Icp::doIteration(const base::Point2dVector& origin, const base::Point2dVector& target,
                      base::Transform2d& transform, const double distance_threshold, double& rms) const
{
  PointPairIndexVector pairs;

  if (!_pair_estimator->findPairs(target, pairs)) {
    LogError() << "Icp::estimateTransform(): error occurred during finding point pairs. Cancel estimation process.";
    return false;
  }  

  try {
    rms = _transform_estimator(origin, target, pairs, std::max(pairs.medianDistance() * 2.0, distance_threshold), transform);

    if (rms >= _max_rms) {
      LogWarn() << "Icp::estimateTransform(): max rms value reached. Cancel estimation process.";
      return false;
    }
  }
  catch (std::bad_function_call& ex) {
    LogError() << "Icp::estimateTransform(): estimate transform can't be called. Maybe function isn't set. Cancel estimation"
                  " process. ex.what(): " << ex.what();
    return false;
  }

  return true;
}

} // end namespace algorithm

} // end namespace francor