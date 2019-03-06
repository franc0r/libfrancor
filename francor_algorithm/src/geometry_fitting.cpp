#include "francor_algorithm/geometry_fitting.h"

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::VectorVector2d& points)
{
    // a line needs minium two points
  if (points.size() < 2)
    return { };

  // calculate the average point
  base::Vector2d avg(0.0, 0.0);

  for (const auto point : points)
    avg += point;

  avg /=  static_cast<double>(points.size());

  // calculate the sum (x - avg.x) * (y - avg.y) and (x - avg.x) * (x - avg.x)
  double sumXY = 0.0;
  double sumX  = 0.0;

  for (const auto point : points)
  {
    sumXY += (point.x() - avg.x()) * (point.y() - avg.y());
    sumX  += (point.x() - avg.x()) * (point.x() - avg.x());
  }

  // m is infinity
  if (sumX == 0.0)
    return { }; // TODO: print error

  // construct line segment and return it
  const double m = sumXY / sumX;
  const double t = avg.y() - m * avg.x();

  return {m, t};
}

} // end namespace algorithm

} // end namespace francor