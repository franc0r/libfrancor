/**
 * Declares a class that represents a rectangle. It is designed for grid or 2d array operations.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. December 2020
 */
#pragma once

#include "francor_base/size.h"
#include "francor_base/point.h"

#include <ostream>

namespace francor {

namespace base {

namespace impl {

template <typename Data>
class Rect
{
public:
  Rect(const Data x, const Data y, const Data size_x, const Data size_y) : _origin(x, y), _size(size_x, size_y) { }
  Rect(const Point2<Data> origin, const Size2<Data> size) : _origin(origin), _size(size) { }

  inline Point2<Data> origin() const { return _origin; }
  inline Size2<Data> size() const { return _size; }

private:
  Point2<Data> _origin{static_cast<Data>(0), static_cast<Data>(0)};
  Size2<Data> _size{static_cast<Data>(0), static_cast<Data>(0)};
};

} // end namespace impl

using Rectd = impl::Rect<double>;
using Rectf = impl::Rect<float>;
using Rectu = impl::Rect<unsigned int>;
using Recti = impl::Rect<int>;

} // end namespace base

} // end namespace francor

namespace std {

template <typename Data>
inline ostream& operator<<(ostream& os, const francor::base::impl::Rect<Data>& rect)
{
  os << "rect: [origin = " << rect.origin() << ", size = " << rect.size() << "]";
  return os;
}

} // end namespace std