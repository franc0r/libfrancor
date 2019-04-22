#include "francor_algorithm/geometry_fitting.h"

#include <algorithm>

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::VectorVector2d& points, const std::vector<std::size_t>& indices)
{
    // a line needs minium two points
  if (points.size() < 2)
    return { };

  // calculate the average point
  base::Vector2d avg(0.0, 0.0);

  if (indices.size() == 0)
  {
    for (const auto point : points)
      avg += point;

    avg /=  static_cast<double>(points.size());
  }
  else
  {
    for (const auto index : indices)
      avg += points[index];
  
    avg /=  static_cast<double>(indices.size());
  }


  // calculate the sum (x - avg.x) * (y - avg.y) and (x - avg.x) * (x - avg.x)
  double sumXY = 0.0;
  double sumX  = 0.0;

  if (indices.size() == 0)
  {
    for (const auto point : points)
    {
      sumXY += (point.x() - avg.x()) * (point.y() - avg.y());
      sumX  += (point.x() - avg.x()) * (point.x() - avg.x());
    }
  }
  else
  {
    for (const auto index : indices)
    {
      sumXY += (points[index].x() - avg.x()) * (points[index].y() - avg.y());
      sumX  += (points[index].x() - avg.x()) * (points[index].x() - avg.x());
    }
  }
  

  // m is infinity
  if (sumX == 0.0)
    return {static_cast<double>(std::numeric_limits<std::size_t>::max()), -static_cast<double>(std::numeric_limits<std::size_t>::max())};

  // construct line segment and return it
  const double m = sumXY / sumX;
  const double t = avg.y() - m * avg.x();

  return {m, t};
}

base::LineSegment fittingLineSegmentFromPoints(const base::VectorVector2d& points, const std::vector<std::size_t>& indices)
{
  if (points.size() < 2 || indices.size() == 1)
  {
    //TODO: print error
    return { };
  }
  
  //TODO: deal with m = 0
  const base::Line line(fittingLineFromPoints(points, indices));

  if (indices.size() == 0)
  {
    const double minY = std::min_element(points.begin(), points.end(), [&] (const base::Vector2d& left, const base::Vector2d& right) { return left.y() < right.y(); })->y();
    const double maxY = std::max_element(points.begin(), points.end(), [&] (const base::Vector2d& left, const base::Vector2d& right) { return left.y() > right.y(); })->y();

    return { base::Vector2d(line.x(minY), minY), base::Vector2d(line.x(maxY), maxY) };
  }
  else
  {
    const double minY = points[*std::min_element(indices.begin(),
                                                 indices.end(), 
                                                 [&] (const std::size_t left, const std::size_t right) { return points[left].y() < points[right].y(); } )
                              ].y();

    const double maxY = points[*std::max_element(indices.begin(),
                                                 indices.end(), 
                                                 [&] (const std::size_t left, const std::size_t right) { return points[left].y() < points[right].y(); } )
                              ].y();             
                                              
    return { base::Vector2d(line.x(minY), minY), base::Vector2d(line.x(maxY), maxY) };
  }
}                                               

} // end namespace algorithm

} // end namespace francor