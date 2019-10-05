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
  Line(const Angle angle = Angle(0.0), const double y0 = 0.0)
    :  _y0(y0),
       _phi(angle)
  {
    assert(!std::isnan(y0) && !std::isinf(y0));
    _phi.normalize();
  }

  Line(const double x0, const double y0)
    : _x0(x0),
      _y0(y0),
      _phi(std::atan2(_y0, _x0))
  {
    _phi.normalize();
  }      

  /**
   * Construct a line from a direction vector and point.
   * 
   * \param v Direction vector of the line. Must have a length of 1.0.
   * \param p One point of the line.
   */
  Line(const Vector2d v, const Vector2d p)
    : _x0(NAN),
      _y0(p.y() - ((p.x() / v.x()) * v.y())),
      _phi(std::atan2(v.y(), v.x()))
  {
    assert(v.norm() <= 1.01 && v.norm() >= 0.99);
    _phi.normalize();
  }

  /**
   * Returns the normal of this line.
   * 
   * \return The normal of this line.
   */
  inline Line n() const { return { _phi + M_PI_2, _y0 }; }

  inline double x0() const noexcept { return _x0;  }
  inline double y0() const noexcept { return _y0;  }
  inline Angle phi() const noexcept { return _phi; }

  /**
   * Calculates the y value for the given x value.
   * 
   * \param x The x value.
   * \return y The y value for given x value.
   */
  inline double y(const double x) const { return std::tan(_phi) * x; }

  /**
   * Calculates the x value for the given y value.
   * 
   * \param y The y value.
   * \return x The x value for the given y value.
   */
  inline double x(const double y) const { return y / std::tan(_phi); }

  /**
   * Calculates the distance along the normal of this line.
   * 
   * \param p The distance will be calculated to that point.
   * \return Distance to the point p.
   */
  double distanceTo(const Vector2d p) const
  {
    const Vector2d hypotenuse = p - Vector2d(0.0, _y0);
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
  Vector2d intersectionPoint(const Line& line) const
  {
    if ((_y0 >= line._y0 && _phi < line._phi)
        ||
        (line._y0 >= _y0 && line._phi < _phi))
    // intersection is on right side, x >= 0
    {
      const Angle gamma(std::abs(_phi - line._phi));
      const Angle target_gamma(M_PI - (M_PI_2 - _phi) - M_PI_2);
      const double gamma_factor = target_gamma / gamma;
      const double diff_y0 = _y0 - line._y0;
      const double pos_y = _y0 + diff_y0 * gamma_factor;
      const double pos_x = std::tan(_phi) * diff_y0 * gamma_factor;

      return { pos_x, pos_y };
    }
    else
    // intersection is on left side, x < 0
    {
      const Angle gamma(std::abs(_phi - line._phi));
      const Angle target_gamma(M_PI - (M_PI_2 - _phi) - M_PI_2);
      const double gamma_factor = target_gamma / gamma;
      const double diff_y0 = _y0 - line._y0;
      const double pos_y = _y0 + diff_y0 * gamma_factor;
      const double pos_x = -std::tan(_phi) * diff_y0 * gamma_factor;

      return { pos_x, pos_y };
    }
  }

private:
  double _x0; //> x-value for y == 0
  double _y0; //> y-value for x == 0 
  Angle _phi; //> angle in rad of the gradient regarding the x-axis
};

using LineVector = std::vector<Line, Eigen::aligned_allocator<Line>>;

} // end namespace base

} // end namespace francor

namespace std
{

inline std::ostream& operator<<(std::ostream& os, const francor::base::Line& line)
{
  os << "line [phi = " << line.phi().radian() << ", y0 = " << line.y0() << "]";

  return os;
}

} // end namespace std