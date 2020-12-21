/**
 * A template class that represents an point in n-dim room.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 5. October 2019
 */
#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>

#include "francor_base/vector.h"

namespace francor {

namespace base {

namespace impl {

template <typename Type>
class Point2
{
public:
  constexpr Point2(const Type x = 0.0, const Type y = 0.0) : _x(x), _y(y) { }

  inline constexpr Type& x() noexcept { return _x; }
  inline constexpr Type x() const noexcept { return _x; }
  inline constexpr Type& y() noexcept { return _y; }
  inline constexpr Type y() const noexcept { return _y; }

  inline constexpr bool isValid() const noexcept
  {
    return !std::isnan(_x) && !std::isnan(_y) && !std::isinf(_x) && !std::isinf(_y);
  }

  inline constexpr Vector2<Type> operator-(const Point2& operant) const { return { operant._x - _x, operant._y - _y }; }
  inline constexpr Point2 operator+(const Vector2<Type>& operant) const { return { _x + operant.x(), _y + operant.y() }; }
  inline constexpr Point2 operator+(const Point2& operant) const { return { _x + operant.x(), _y + operant.y() }; }
  inline constexpr Point2& operator+=(const Point2& operant) { _x += operant._x; _y += operant._y; return *this; }
  inline constexpr Point2& operator/=(const double operant) { _x /= operant; _y /= operant; return *this; }

  inline constexpr bool operator==(const Point2& operant) const { return _x == operant._x && _y == operant._y; }
  inline constexpr bool operator!=(const Point2& operant) const { return !this->operator==(operant); }

private:
  Type _x;
  Type _y;
};

} // end namespace impl

template <typename Data>
using Point2 = impl::Point2<Data>;
using Point2d = impl::Point2<double>;

using Point2dVector = std::vector<Point2d>;

} // end namespace base

} // end namespace francor

namespace std
{

template <typename Type>
inline ostream& operator<<(ostream& os, const francor::base::Point2<Type>& point)
{
  os << "[x = " << point.x() << ", y = " << point.y() << "]";

  return os;
}

template <typename Type>
inline ostream& operator<<(ostream& os, const vector<francor::base::Point2<Type>>& points)
{
  os << "point vector:" << std::endl;
  os << "size   = " << points.size() << std::endl;
  os << "data[] = ";

  for (const auto& point : points) {
    os << point << " ";
  } 

  return os;
}

} // end namespace std