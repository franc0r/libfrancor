/**
 * ICP implementation.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. October 2019
 */

#pragma once

#include <memory>
#include <functional>

#include <francor_base/point.h>
#include <francor_base/transform.h>
#include <francor_algorithm/point_pair_estimator.h>

namespace francor {

namespace algorithm {

using TransformEstimationFunction = std::function<double(const base::Point2dVector& dataset_a,
                                                         const base::Point2dVector& dataset_b,
                                                         const PointPairIndexVector& pair_indices,
                                                         const double max_distance,
                                                         base::Transform2d& transform)>;

// \todo make icp more generic

class Icp
{
public:
  Icp(std::unique_ptr<PointPairEstimator>&& pair_estimator, const TransformEstimationFunction& transform_estimator)
    : _pair_estimator(std::move(pair_estimator)),
      _transform_estimator(transform_estimator)
  { }

  inline void setMaxIterations(const std::size_t max_iterations) noexcept { _max_iterations = max_iterations; }
  inline void setMaxRms(const double max_rms) noexcept { assert(max_rms >= 0.0); _max_rms = max_rms; }
  inline void setTerminationRms(const double value) noexcept { _termination_rms = value; }
  inline std::size_t getMaxIterations() const noexcept { return _max_iterations; }
  inline double getMaxRms() const noexcept { return _max_rms; }
  inline double getTerminationRms() const noexcept { return _termination_rms; }

  bool estimateTransform(const base::Point2dVector& origin, const base::Point2dVector& target, base::Transform2d& transform) const;

private:
  bool doIteration(const base::Point2dVector& origin, const base::Point2dVector& target,
                   base::Transform2d& transform, double& rms) const;

  std::unique_ptr<PointPairEstimator> _pair_estimator;
  TransformEstimationFunction _transform_estimator;
  std::size_t _max_iterations = 100;
  double _max_rms = 1.0;
  double _termination_rms = 1.0;
};

} // end namespace algorithm

} // end namespace francor