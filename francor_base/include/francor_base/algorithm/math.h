/**
 * Math functions that aren't in std.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <utility>

namespace francor {

namespace base {

namespace algorithm {

namespace math {

template <typename Type, std::size_t... K>
inline Type fak(std::index_sequence<K...>)
{
  return ((K + 1) * ...);
}

template <std::size_t N, typename Type, typename Indices = std::make_index_sequence<N>>
inline Type fak()
{
  return fak<Type>(Indices{});
}

template <typename Type>
inline Type fak(const Type value)
{
  Type result = 1;

  for (Type k = 1; k <= value; ++k) {
    result *= k;
  }

  return result;
}

} // end namespace point

} // end namespace algorithm

} // end namespace base

} // end namespace francor