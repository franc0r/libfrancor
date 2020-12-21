/**
 * Implements a class that represents a size in 2d. It is a template so it can be used for several data types.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. December 2020
 */
#pragma once

#include <cstdint>
#include <ostream>

namespace francor {

namespace base {

namespace impl {

template <typename Data>
class Size2
{
public:
  constexpr Size2(const Data x = static_cast<Data>(0), const Data y = static_cast<Data>(0)) : _x(x), _y(y) { }

  constexpr inline Data x() const { return _x; }
  constexpr inline Data y() const { return _y; }

  constexpr inline Size2& operator/=(const Data rhs) { _x /= rhs; _y /= rhs; return *this; }
  constexpr inline bool operator==(const Size2& rhs) const { return _x == rhs._x && _y == rhs._y; }
  constexpr inline bool operator!=(const Size2& rhs) const { return !operator==(rhs); }

private:
  Data _x{static_cast<Data>(0)};
  Data _y{static_cast<Data>(0)};
};

template <typename Data>
class Size3 : public Size2<Data>
{
public:
  constexpr Size3(const Data x, const Data y, const Data z) : Size2<Data>(x, y), _z(z) { }

  constexpr inline Data z() const { return _z; }

  constexpr inline Size3& operator/=(const Data rhs) { Size2<Data>::operator/=(rhs); _z /= rhs; return *this; }
  constexpr inline bool operator==(const Size3& rhs) const { return Size2<Data>::operator==(rhs) && _z == rhs._z; }
  constexpr inline bool operator!=(const Size3& rhs) const { return !operator==(rhs); }

private:
  Data _z;
};

} // end namespace impl

// aliases for common size types
template <typename Data>
using Size2  = impl::Size2<Data>;
using Size2u = impl::Size2<std::size_t>;
using Size3u = impl::Size3<std::size_t>;
using Size2d = impl::Size2<double>;
using Size3d = impl::Size3<double>;

} // end namespace base

} // end namespace francor


namespace std {

template <typename Data>
inline ostream& operator<<(ostream& os, const francor::base::impl::Size2<Data>& size)
{
  os << "size [x = " << size.x() << ", y = " << size.y() << "]";
  return os;
}

template <typename Data>
inline ostream& operator<<(ostream& os, const francor::base::impl::Size3<Data>& size)
{
  os << "size [x = " << size.x() << ", y = " << size.y() << ", z = " << size.z() << "]";
  return os;
}

} // end namespace std