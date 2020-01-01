#include "francor_algorithm/geometry_fitting.h"

#include <algorithm>

namespace francor {

namespace algorithm {

base::Line fittingLineFromPoints(const base::Point2dVector& points, const std::vector<std::size_t>& indices)
{
    // a line needs minium two points
  if (points.size() < 2)
    return { };

  // calculate the average point
  base::Point2d avg(0.0, 0.0);

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

  // happens only if x of all points are equal (vertical line)
  if (sumX == 0.0)
    return { M_PI_2, avg };
  
  return { std::atan2(sumXY, sumX), avg };
}

base::LineSegment fittingLineSegmentFromPoints(const base::Point2dVector& points, const std::vector<std::size_t>& indices)
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
    const double minY = std::min_element(points.begin(), points.end(), [&] (const base::Point2d& left, const base::Point2d& right) { return left.y() < right.y(); })->y();
    const double maxY = std::max_element(points.begin(), points.end(), [&] (const base::Point2d& left, const base::Point2d& right) { return left.y() > right.y(); })->y();

    return { base::Point2d(line.x(minY), minY), base::Point2d(line.x(maxY), maxY) };
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

    if (minY != maxY)
    {
      if (std::abs(line.phi()) == M_PI_2)
      {
        return { base::Point2d(line.x0(), minY), base::Point2d(line.x0(), maxY) };
      }
      // else: normal case, expect valid x value
      // TODO: search for min and max x values, too.
      base::Point2d p0(line.x(minY), minY);
      base::Point2d p1(line.x(maxY), maxY);
      return { p0, p1 };
    }
    else
    // horizontal line -> search using x values
    {
      const double minX = points[*std::min_element(indices.begin(),
                                                   indices.end(), 
                                                   [&] (const std::size_t left, const std::size_t right) { return points[left].x() < points[right].x(); } )
                                ].x();

      const double maxX = points[*std::max_element(indices.begin(),
                                                   indices.end(), 
                                                   [&] (const std::size_t left, const std::size_t right) { return points[left].x() < points[right].x(); } )
                                ].x();

      base::Point2d p0(minX, line.y(minX));
      base::Point2d p1(maxX, line.y(maxX));
      return { p0, p1 };
    }
  }
}                

std::optional<std::vector<base::NormalizedAngle>> estimateNormalsFromOrderedPoints(const base::Point2dVector& points, const int n)
{
  if (n % 2 != 1) {
    base::LogError() << "estimateNormalsFromOrderedPoints(): argument n must be odd. n = " << n;
    return std::nullopt;
  }

  std::vector<base::NormalizedAngle> normals;
  normals.reserve(points.size());
  std::vector<std::size_t> indices(n);
  indices.reserve(n);

  for (int p = 0; p < static_cast<int>(points.size()); ++p) {
    for (int i = p - n / 2; i < n; ++i) {
      if (i > 0) indices.push_back(i);
    }

    normals.push_back(base::NormalizedAngle(90.0) + fittingLineFromPoints(points, indices).phi());
    indices.clear();
  }

  return normals;
}

} // end namespace algorithm

} // end namespace francor