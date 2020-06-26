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
  template <typename SourceType>
  constexpr Point2dImpl(const Point2dImpl<SourceType>& source)
    : _x(static_cast<Type>(source.x())),
      _y(static_cast<Type>(source.y()))
  {
    static_assert(std::is_floating_point<Type>::value
                  ||
                  (std::is_integral<Type>::value && std::is_signed<Type>::value)
                  ||
                  (std::is_integral<Type>::value && std::is_unsigned<Type>::value && std::is_unsigned<Type>()),
                  "convertion is dangerous and is not supported");
  }
  constexpr Point2dImpl(const Point2dImpl&) = default;
  constexpr Point2dImpl(Point2dImpl&&) = default;
  ~Point2dImpl() = default;

  inline constexpr Type& x() noexcept { return _x; }
  inline constexpr Type x() const noexcept { return _x; }
  inline constexpr Type& y() noexcept { return _y; }
  inline constexpr Type y() const noexcept { return _y; }

  inline constexpr bool isValid() const noexcept
  {
    return !std::isnan(_x) && !std::isnan(_y) && !std::isinf(_x) && !std::isinf(_y);
  }

  inline constexpr Vector2<Type> operator-(const Point2dImpl& operant) const { return { operant._x - _x, operant._y - _y }; }
  inline constexpr Point2dImpl operator+(const Vector2<Type>& operant) const { return { _x + operant.x(), _y + operant.y() }; }
  inline constexpr Point2dImpl operator+(const Point2dImpl& operant) const { return { _x + operant.x(), _y + operant.y() }; }
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

using Point2u = Point2dImpl<unsigned int>;
using Point2i = Point2dImpl<int>;
using Point2f = Point2dImpl<float>;
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

template <typename Type>
inline ostream& operator<<(ostream& os, const vector<francor::base::Point2dImpl<Type>>& points)
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