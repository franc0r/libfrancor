/**
 * A template class that represents a size.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include <ostream>

namespace francor {

namespace base {

template <typename Type>
struct Size2Impl
{
public:
  constexpr Size2Impl(const Type width_ = static_cast<Type>(0), const Type height_ = static_cast<Type>(0))
    : width(width_),
      height(height_)
  { }
  template <typename SourceType>
  constexpr Size2Impl(const Size2Impl<SourceType> source)
    : width(static_cast<Type>(source.width)),
      height(static_cast<Type>(source.height))
  { }

  Type width;
  Type height;
};

using Size2i = Size2Impl<int>;
using Size2u = Size2Impl<unsigned int>;
using Size2d = Size2Impl<double>;
using Size2f = Size2Impl<float>;

} // end namespace base

} // end namespace francor


namespace std
{

template <typename Type>
inline ostream& operator<<(ostream& os, const francor::base::Size2Impl<Type>& size)
{
  os << "[w = " << size.width << ", h = " << size.height << "]";

  return os;
}

} // end namespace std