/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <Eigen/Dense>

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
  }
  /**
   * Construct a line from a direction vector and point.
   * 
   * \param v Direction vector of the line. Must have a length of 1.0.
   * \param p One point of the line.
   */
  Line(const Eigen::Vector2d v, const Eigen::Vector2d p)
    : v_(v),
      p_(p),
      m_(v.y() / v.x()),
      t_(p.y() - m_ * p.x())
  {
    assert(v_.norm() <= 1.0);
  }

  /**
   * Returns the normal of this line.
   * 
   * \param p The normal points in direction of p.
   * \return The normal of this line.
   */
  Eigen::Vector2d n(const Eigen::Vector2d p) const
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
  Eigen::Vector2d n(void) const
  {
    return { -v_.y(), v_.x() };
  }

  /**
   * Returns the direction vector v.
   * 
   * \return The direction vector v.
   */
  inline const Eigen::Vector2d& v(void) const noexcept { return v_; }

  /**
   * Returns the reference point p. Actually it is more a offset point.
   * 
   * \return The reference point p.
   */
  inline const Eigen::Vector2d& p(void) const noexcept { return p_; }

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
   * Calculates the distance along the normal of this line.
   * 
   * \param p The distance will be calculated to that point.
   * \return Distance to the point p.
   */
  double distanceTo(const Eigen::Vector2d p) const
  {
    return (this->intersectionPoint(Line(this->n(p) * -1.0, p)) - p).norm();
  }

  /**
   * Calculates the intersection point of two lines.
   * 
   * \param line Other line.
   * \return Intersection point of that two lines.
   */
  Eigen::Vector2d intersectionPoint(const Line& line) const
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

    return { x, y };
  }

private:
  Eigen::Vector2d v_;
  Eigen::Vector2d p_;
  double m_;
  double t_;
};

} // end namespace perception

} // end namespace francor