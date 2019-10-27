/**
 * Collection of function to estimate transform between data sets.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#include "francor_algorithm/estimate_transform.h"

#include <francor_base/log.h>
#include <francor_base/vector.h>

namespace francor {

namespace algorithm {

using francor::base::LogError;

double estimateTransform(const base::Point2dVector& dataset_a,
                         const base::Point2dVector& dataset_b,
                         const PointPairIndexVector& pair_indices,
                         base::Transform2d& transform)
{
  if (dataset_a.empty() || dataset_b.empty())
  {
    LogError() << "estimateTransform(): each point dataset must minium contain one point. Can't estimate transformation.";
    return -1.0;
  }
  if (pair_indices.empty())
  {
    LogError() << "estimateTransform(): minimum one point pair is required. Can't estimate transformation.";
    return -1.0;
  }
  // \todo Add check for pair indices. Check if the indices relate to the two datasets.
  using base::Point2d;
  using base::Vector2d;

  // calculate centroid of each dataset
  Point2d centroid_set_a(0.0, 0.0);
  Point2d centroid_set_b(0.0, 0.0);
  double rms = 0.0;

  for (const auto& pair : pair_indices)
  {
    const auto& point_a = dataset_a[pair.first ];
    const auto& point_b = dataset_b[pair.second];
    centroid_set_a += point_a;
    centroid_set_b += point_b;
    rms += (point_b - point_a).norm();
  }

  centroid_set_a /= static_cast<double>(pair_indices.size());
  centroid_set_b /= static_cast<double>(pair_indices.size());
  rms            /= static_cast<double>(pair_indices.size());

  // calculate nominator and denominator
  double d_nominator = 0.0;
  double d_denominator = 0.0;

  for (const auto& pair : pair_indices)
  {
    const Vector2d dFC_a(dataset_a[pair.first ] - centroid_set_a);
    const Vector2d dFC_b(dataset_b[pair.second] - centroid_set_b);
    d_nominator   += dFC_a.y() * dFC_b.x() - dFC_a.x() * dFC_b.y();
    d_denominator += dFC_a.x() * dFC_b.x() + dFC_a.y() * dFC_b.y();
  }

  // calculate rotation
  transform.setRotation(std::atan2(d_nominator, d_denominator));

  // rotate centroid of dataset b and estimate translation
  centroid_set_b = transform.rotation() * centroid_set_b;
  transform.setTranslation(centroid_set_b - centroid_set_a);

  return rms;
}

} // end namespace algorithm

} // end namespace francor