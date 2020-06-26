/**
 * A template class that represents a rectangle.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include "francor_base/point.h"
#include "francor_base/size.h"

namespace francor {

namespace base {

template <typename Type>
class Rect2Impl
{
public:
  constexpr Rect2Impl(const Type x, const Type y, const Type width, const Type height)
    : _origin(x, y),
      _size(width, height)
  { }
  constexpr Rect2Impl(const Point2dImpl<Type> origin = {0, 0}, const Size2Impl<Type> size = {0, 0})
    : _origin(origin),
      _size(size)
  { }
  template <typename SourceType>
  constexpr Rect2Impl(const Rect2Impl<SourceType>& source)
    : _origin(source.origin()),
      _size(source.size())
  { }

  constexpr inline Type x() const noexcept { return _origin.x(); }
  constexpr inline Type y() const noexcept { return _origin.y(); }
  constexpr inline Type width() const noexcept { return _size.width; }
  constexpr inline Type height() const noexcept { return _size.height; }
  constexpr inline Point2dImpl<Type> origin() const noexcept { return _origin; }
  constexpr inline Size2Impl<Type> size() const noexcept { return _size; }

  constexpr inline Type xMax() const { return std::max(_origin.x() + _size.width , 1u) - 1; }
  constexpr inline Type yMax() const { return std::max(_origin.y() + _size.height, 1u) - 1; }

  inline void setOrigin(const Point2dImpl<Type> origin) { _origin = origin; }
  inline void setWidth(const Type width) { _size.width = width; }
  inline void setHeight(const Type height) { _size.height = height; }

private:
  Point2dImpl<Type> _origin;
  Size2Impl<Type> _size;
};

using Rect2u = Rect2Impl<unsigned int>;
using Rect2i = Rect2Impl<int>;
using Rect2f = Rect2Impl<float>;
using Rect2d = Rect2Impl<double>;

} // end namespace base

} // end namespace francor


namespace std
{

template <typename Type>
inline ostream& operator<<(ostream& os, const francor::base::Rect2Impl<Type>& rect)
{
  os << "[origin = " << rect.origin() << ", size = " << rect.size() << "]";

  return os;
}

} // end namespace std