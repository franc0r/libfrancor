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
  double rms = 0.0;

  // iterate until max iterations is reached or a other criterion is fulfilled
  for (std::size_t iteration = 0; iteration < _max_iterations; ++iteration)
  {
    // do iteration and estimate transformation
    if (!this->doIteration(origin, moved_points, transform, rms)) {
      return false;
    }

    // check if estimated transformation is good enough
    if (rms <= _termination_rms) {
      return true;
    }

    // transform points
    for (auto& point : moved_points)
      point = transform * point;
  }

  return true;
}

bool Icp::doIteration(const base::Point2dVector& origin, const base::Point2dVector& target,
                      base::Transform2d& transform, double& rms) const
{
  PointPairIndexVector pairs;

  if (!_pair_estimator->findPairs(target, pairs)) {
    LogError() << "Icp::estimateTransform(): error occurred during finding point pairs. Cancel estimation process.";
    return false;
  }  

  try {
    rms = _transform_estimator(origin, target, pairs, transform);

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