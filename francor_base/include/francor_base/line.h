/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/log.h"
#include "francor_base/angle.h"
#include "francor_base/point.h"

#include <ostream>

namespace francor {

namespace base {

using francor::base::Angle;

class Line
{
public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /**
   * Default constructor.
   * 
   * \param angle gradient of the line as angle in rad.
   * \param y0 y-value of this line for x == 0.
   * \param x0 x-value of this line for y == 0. Note: that value is ussely set by this class automatically, but
   *        in case the angle is close to pi/2 or -pi/2 it helps a lot.
   */
  Line(const Angle& angle = Angle(0.0), const Point2d& point = Point2d(0.0, 0.0))
    : _phi(angle),
      _p(point)
  {

  }

  /**
   * Returns the normal of this line.
   * 
   * \return The normal of this line.
   */
  inline Vector2d n() const { return { std::cos(_phi + M_PI_2), std::sin(_phi + M_PI_2) }; }
  inline Vector2d v() const { return { std::cos(_phi),  std::sin(_phi) }; }
  inline double x0() const { const double delta_x = _p.y() / std::tan(_phi); return _phi >= 0.0 ? _p.x() - delta_x : _p.x() + delta_x; }
  inline double y0() const { const double delta_y = _p.x() * std::tan(_phi); return _phi >= 0.0 ? _p.y() - delta_y : _p.x() + delta_y; }
  inline Angle phi() const { return _phi; }
  inline const Point2d p() const noexcept { return _p; }

  /**
   * Calculates the y value for the given x value.
   * 
   * \param x The x value.
   * \return y The y value for given x value.
   */
  inline double y(const double x) const { return std::tan(_phi) * x + _p.y(); }

  /**
   * Calculates the x value for the given y value.
   * 
   * \param y The y value.
   * \return x The x value for the given y value.
   */
  inline double x(const double y) const { return y / std::tan(_phi) + _p.x(); }

  /**
   * Calculates the distance along the normal of this line.
   * 
   * \param p The distance will be calculated to that point.
   * \return Distance to the point p.
   */
  double distanceTo(const Point2d p) const
  {
    const Vector2d hypotenuse = p - _p;
    const double hypotenuse_length = hypotenuse.norm();
    const double angle_hypotenuse = std::atan2(hypotenuse.y(), hypotenuse.x());
    const double gegenkathete_length = std::abs(std::sin(angle_hypotenuse - _phi) * hypotenuse_length);

    return gegenkathete_length;
  }

  /**
   * Calculates the intersection point of two lines.
   * 
   * \param line Other line.
   * \return Intersection point of that two lines.
   */
  Point2d intersectionPoint(const Line& line) const
  {
    // p_x = intersection point
    //
    // p_x = p0 + s0 * v0 = p1 + s1 * v1
    //
    // ==> p_x.x = p0.x + s0 * v0.x = p1.x + s1 * v1.x
    //     p_x.y = p0.y + s0 * v0.y = p1.y + s1 * v1.y
    //
    // ==> s1 = (p0.x + s0 * v0.x - p1.x) / v1.x
    //
    // ==> p0.y + s0 * v0.y = p1.y + (p0.x + s0 * v0.x - p1.x) * v1.y / v1.x
    //     s0 * v0.y - s0 * (v0.x / v1.y) / v1.x = p1.y - p0.y + (p0.x - p1.x) * v1.y / v1.x
    //     s0 * v0.y * v1.x - s0 * v0.x * v1.y = (p1.y - p0.y) * v1.x + (p0.x - p1.x) * v1.y
    //
    //          (p1.y - p0.y) * v1.x + (p0.x - p1.x) * v1.y      a
    // ==> s0 = -------------------------------------------  =  ---
    //                  v0.y * v1.x - v0.x * v1.y                b
    // 
    const auto v0 = this->v();
    const auto v1 = line.v();
    const double a = (line._p.y() - _p.y()) * v1.x() + (_p.x() - line._p.x()) * v1.y();
    const double b = v0.y() * v1.x() - v0.x() * v1.y();
    const double s0 = a / b;

    return _p + v0 * s0;
  }

  static Line createFromVectorAndPoint(const Vector2d& v, const Point2d& p)
  {
    assert(v.norm() - 1.0 <= 1e-6);
    return { std::atan2(v.y(), v.x()), p };
  }
  static Line createFromTwoPoints(const Point2d& p0, const Point2d& p1)
  {
    assert(p0 != p1);
    const auto v = (p1 - p0).normalized();
    return { std::atan2(v.y(), v.x()), p0 };
  }

private:
  NormalizedAngle _phi; //> angle in rad of the gradient regarding the x-axis
  Point2d _p; //> center point of this line
};

using LineVector = std::vector<Line>;

} // end namespace base

} // end namespace francor

namespace std
{

inline std::ostream& operator<<(std::ostream& os, const francor::base::Line& line)
{
  os << "line [phi = " << line.phi().radian() << ", p = " << line.p() << "]";

  return os;
}

} // end namespace std