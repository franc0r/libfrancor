/**
 * Represent a line segment and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "line.h"

#include <ostream>

namespace francor {

namespace base {

class LineSegment
{
public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
  /**
   * Default ctor. The object is invalid.
   */
  LineSegment(void) = default;

  /**
   * Constructs the segment from to two points.
   * 
   * \param p0 The begin of the segment.
   * \param p1 The end of the segment.
   */
  LineSegment(const Vector2d p0, const Vector2d p1)
    : valid_(true),
      line_((p1 - p0).normalized(), p0),
      p0_(p0),
      p1_(p1)
  {

  }

  inline const Vector2d& p0(void) const noexcept { return p0_; }
  inline const Vector2d& p1(void) const noexcept { return p1_; }
  inline const Line& line(void) const noexcept { return line_; }
  inline bool valid(void) const noexcept { return valid_; }

private:
  bool valid_ = false;
  Line line_;
  Vector2d p0_;
  Vector2d p1_;
};

using LineSegmentVector = std::vector<LineSegment, Eigen::aligned_allocator<LineSegment>>;

} // end namespace base

} // end namespace francor

namespace std
{

std::ostream& operator<<(std::ostream& os, const francor::base::LineSegment& segment)
{
  os << "[line: " << segment.line() << " | p0: x = " << segment.p0().x() << ", y = " << segment.p0().y()
     << " | p1: x = " << segment.p1().x() << ", y = " << segment.p1().y() << "]";

  return os;
}

} // end namespace std
