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
  double rms = std::numeric_limits<double>::max();
  transform.setRotation(0.0);
  transform.setTranslation({ 0.0, 0.0 });

  // iterate until max iterations is reached or a other criterion is fulfilled
  for (std::size_t iteration = 0; iteration < _max_iterations; ++iteration)
  {
    base::Transform2d current_transform;
    double current_rms;

    // do iteration and estimate transformation
    if (!this->doIteration(origin, moved_points, current_transform, current_rms)) {
      return false;
    }
    // std::cout << "current rms = " << current_rms << std::endl;
    // check if estimated transformation is good enough
    if (current_rms <= _termination_rms || current_rms >= rms) {
      return true;
    }

    // integrate current iteration results
    rms = current_rms;
    // std::cout << "estimated current_transform: " << current_transform << std::endl;
    transform = transform * current_transform;
    // std::cout << "rms = " << rms << std::endl;
    // std::cout << "estimated transform: " << transform << std::endl;

    // transform points
    current_transform = current_transform.inverse();

    for (auto& point : moved_points)
      point = current_transform * point;

    // std::cout << "moved point " << moved_points << std::endl;
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
    // std::cout << pairs << std::endl;
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