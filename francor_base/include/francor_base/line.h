/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_base/vector.h"

#include <iostream>

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
  Line(const double m = 0.0, const double t = 0.0)
    : v_(1.0, m),
      p_(0.0, t),
      m_(m),
      t_(t)
  {
    v_.normalize();
    assert(std::abs(m_) <= static_cast<double>(std::numeric_limits<std::size_t>::max()));
    assert(std::abs(t_) <= static_cast<double>(std::numeric_limits<std::size_t>::max()));
  }
  /**
   * Construct a line from a direction vector and point.
   * 
   * \param v Direction vector of the line. Must have a length of 1.0.
   * \param p One point of the line.
   */
  Line(const Vector2d v, const Vector2d p)
    : v_(v),
      p_(p),
      // if v.x is close to zero use limit or zero
      m_(std::abs(v.x()) < 1e-6 ? (std::abs(v.y()) < 1e-6 ? 0.0 : static_cast<double>(std::numeric_limits<std::size_t>::max())) : v.y() / v.x()),
      t_(p.y() - m_ * p.x())
  {
    if (std::isinf(t_))
      t_ = -static_cast<double>(std::numeric_limits<std::size_t>::max());
    // limit m_ to size_t max limit. That is the maximum expected number of cols or rows of an image.
    // at moment the is the supported range
    if (std::abs(m_) > static_cast<double>(std::numeric_limits<std::size_t>::max()))
    {
      const double positive = m_ >= 0.0;
      m_ = static_cast<double>(std::numeric_limits<std::size_t>::max()) * (positive ? 1.0 : -1.0);
    }
    if (std::abs(t_) > static_cast<double>(std::numeric_limits<std::size_t>::max()))
    {
      const double positive = t_ >= 0.0;
      t_ = static_cast<double>(std::numeric_limits<std::size_t>::max()) * (positive ? 1.0 : -1.0);
    }

    assert(v_.norm() <= 1.01);
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
    return { -v_.y(), v_.x() };
  }

  /**
   * Returns the direction vector v.
   * 
   * \return The direction vector v.
   */
  inline const Vector2d& v(void) const noexcept { return v_; }

  /**
   * Returns the reference point p. Actually it is more a offset point.
   * 
   * \return The reference point p.
   */
  inline const Vector2d& p(void) const noexcept { return p_; }

  /**
   * Returns the gradient m of the line.
   * 
   * \return The gradient m.
   */
  inline double m(void) const noexcept { return m_; }

  /**
   * Returns the offset t of the line.
   * 
   * \return The offset t.
   */
  inline double t(void) const noexcept { return t_; }

  /**
   * Calculates the y value for the given x value.
   * 
   * \param x The x value.
   * \return y The y value for given x value.
   */
  inline double y(const double x) const noexcept
  {
    return m_ * x + t_;
  }

  /**
   * Calculates the x value for the given y value.
   * 
   * \param y The y value.
   * \return x The x value for the given y value.
   */
  inline double x(const double y) const
  {
    // if m is zero it exists inf number of solutions --> return just zero as x value
    if (m_ == 0.0)
      return 0.0;

    return (y - t_) / m_;
  }

  /**
   * Calculates the distance along the normal of this line.
   * 
   * \param p The distance will be calculated to that point.
   * \return Distance to the point p.
   */
  double distanceTo(const Vector2d p) const
  {
    // special case m = 0.0
    if (m_ == 0.0)
      return std::abs(p.y() - t_);

    return (this->intersectionPoint(Line(this->n(p) * -1.0, p)) - p).norm();
  }

  /**
   * Calculates the intersection point of two lines.
   * 
   * \param line Other line.
   * \return Intersection point of that two lines.
   */
  Vector2d intersectionPoint(const Line& line) const
  {
    // m_1 * x + t_1 = m_2 * x + t_2
    //     t_1 - t_2 = m_2 * x - m_1 * x = (m_2 - m_1) * x
    //
    // ==> x = (t_1 - t_2) / (m_2 - m_1)
    const double subM = line.m_ - m_;

    if (subM == 0.0)
      return p_;

    const double x = (t_ - line.t_) / subM;
    const double y = m_ * x + t_;

    assert(!std::isinf(x));
    assert(!std::isinf(y));

    return { x, y };
  }

private:
  Vector2d v_;
  Vector2d p_;
  double m_;
  double t_;
};

using LineVector = std::vector<Line, Eigen::aligned_allocator<Line>>;

} // end namespace base

} // end namespace francor