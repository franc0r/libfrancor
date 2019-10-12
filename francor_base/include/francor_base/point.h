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

template <typename Type>
class Point2dImpl
{
public:
  constexpr Point2dImpl(const Type x = 0.0, const Type y = 0.0) : _x(x), _y(y) { }
  Point2dImpl(const Point2dImpl&) = default;
  Point2dImpl(Point2dImpl&&) = default;
  ~Point2dImpl() = default;

  inline constexpr Type& x() noexcept { return _x; }
  inline constexpr Type x() const noexcept { return _x; }
  inline constexpr Type& y() noexcept { return _y; }
  inline constexpr Type y() const noexcept { return _y; }

  inline constexpr Vector2<Type> operator-(const Point2dImpl& operant) const { return { operant._x - _x, operant._y - _y }; }
  inline constexpr Point2dImpl operator+(const Vector2<Type>& operant) const { return { _x + operant.x(), _y + operant.y() }; }
  inline constexpr Point2dImpl& operator+=(const Point2dImpl& operant) { _x += operant._x; _y += operant._y; return *this; }
  inline constexpr Point2dImpl& operator/=(const double operant) { _x /= operant; _y /= operant; return *this; }

  inline constexpr bool operator==(const Point2dImpl& operant) const { return _x == operant._x && _y == operant._y; }
  inline constexpr bool operator!=(const Point2dImpl& operant) const { return !this->operator==(operant); }

  inline constexpr Point2dImpl& operator=(const Point2dImpl& operant) = default;
  inline constexpr Point2dImpl& operator=(Point2dImpl&& operant) = default;

private:
  Type _x;
  Type _y;
};

using Point2d = Point2dImpl<double>;

using Point2dVector = std::vector<Point2d>;

} // end namespace base

} // end namespace francor

namespace std
{

template <typename Type>
inline ostream& operator<<(ostream& os, const francor::base::Point2dImpl<Type>& point)
{
  os << "[x = " << point.x() << ", y = " << point.y() << "]";

  return os;
}

} // end namespace std