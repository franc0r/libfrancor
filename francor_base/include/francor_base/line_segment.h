/**
 * Represent a line segment and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "line.h"

namespace francor {

namespace perception {

class LineSegment
{
public:
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
  LineSegment(const Eigen::Vector2d p0, const Eigen::Vector2d p1)
    : valid_(true),
      line_((p1 - p0).normalized(), p0),
      p0_(p0),
      p1_(p1)
  {

  }

  inline const Eigen::Vector2d& p0(void) const noexcept { return p0_; }
  inline const Eigen::Vector2d& p1(void) const noexcept { return p1_; }
  inline const Line& line(void) const noexcept { return line_; }
  inline bool valid(void) const noexcept { return valid_; }

private:
  bool valid_ = false;
  Line line_;
  Eigen::Vector2d p0_;
  Eigen::Vector2d p1_;
};

} // end namespace perception

} // end namespace francor