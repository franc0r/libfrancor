/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/log.h"

#include <ostream>

namespace francor {

namespace base {

class Line
{
public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /**
   * Default constructor.
   * 
   * \param m Gradient of the line.
   * \param t Offset of the line.
   */
  Line(const double angle = 0.0, const double t = 0.0)
    : _v(std::cos(angle), std::sin(angle)),
      _p(0.0, t),
      _t(t),
      _angle(angle)
  {
    assert(_v.norm() <= 1.01 && _v.norm() >= 0.99);
  }
  /**
   * Construct a line from a direction vector and point.
   * 
   * \param v Direction vector of the line. Must have a length of 1.0.
   * \param p One point of the line.
   */
  Line(const Vector2d v, const Vector2d p)
    : _v(v),
      _p(p),
      _t(p.y() - ((p.x() / v.x()) * v.y())),
      _angle(std::atan2(v.y(), v.x()))
  {
    assert(_v.norm() <= 1.01 && _v.norm() >= 0.99);
    assert(_angle >= -M_PI && _angle <= M_PI);
  }

  /**
   * Returns the normal of this line.
   * 
   * \param p The normal points in direction of p.
   * \return The normal of this line.
   */
  Vector2d n(const Vector2d p) const
  {
    if (p.y() >= this->y(p.x()))
      return this->n();
    else
      return this->n() * -1.0;
  }
 /**
   * Returns the normal of this line.
   * 
   * \return The normal of this line.
   */
  Vector2d n(void) const
  {
    return { -_v.y(), _v.x() };
  }

  /**
   * Returns the direction vector v.
   * 
   * \return The direction vector v.
   */
  inline const Vector2d& v(void) const noexcept { return _v; }

  /**
   * Returns the reference point p. Actually it is more a offset point.
   * 
   * \return The reference point p.
   */
  inline const Vector2d& p(void) const noexcept { return _p; }

  /**
   * Returns the gradient m of the line.
   * 
   * \return The gradient m.
   */
  inline double m(void) const noexcept { return _v.y() / _v.x(); }

  /**
   * Returns the offset t of the line.
   * 
   * \return The offset t.
   */
  inline double t(void) const noexcept { return _t; }

  /**
   * Calculates the y value for the given x value.
   * 
   * \param x The x value.
   * \return y The y value for given x value.
   */
  inline double y(const double x) const noexcept
  {
    return this->m() * x + _t;
  }

  /**
   * Calculates the x value for the given y value.
   * 
   * \param y The y value.
   * \return x The x value for the given y value.
   */
  inline double x(const double y) const
  {
    return (y - _t) / this->m();
  }

  /**
   * Calculates the distance along the normal of this line.
   * 
   * \param p The distance will be calculated to that point.
   * \return Distance to the point p.
   */
  double distanceTo(const Vector2d p) const
  {
    const Vector2d hypotenuse = p - _p;
    const double hypotenuse_length = hypotenuse.norm();
    const double angle_hypotenuse = std::atan2(hypotenuse.y(), hypotenuse.x());
    const double gegenkathete_length = std::abs(std::sin(std::abs(angle_hypotenuse - _angle)) * hypotenuse_length);

    return gegenkathete_length;
  }

  /**
   * Calculates the intersection point of two lines.
   * 
   * \param line Other line.
   * \return Intersection point of that two lines.
   */
  Vector2d intersectionPoint(const Line& line) const
  {
    const double gegenkathete_length = this->distanceTo(line._p);
    const double alpha = std::abs(line._angle - _angle);
    const double hypotenuse_length = gegenkathete_length / std::sin(alpha);

    return line._p + line._v * hypotenuse_length;
  }

private:
  Vector2d _v;
  Vector2d _p;
  double _t;
  double _angle;
};

using LineVector = std::vector<Line, Eigen::aligned_allocator<Line>>;

} // end namespace base

} // end namespace francor

namespace std
{

std::ostream& operator<<(std::ostream& os, const francor::base::Line& line)
{
  os << "[m = " << line.m() << ", t = " << line.t() << "]";

  return os;
}

} // end namespace std