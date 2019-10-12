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

namespace francor {

namespace base {

template <std::size_t Element, typename Type, std::size_t Dimension, typename>
class PointImpl;

template <std::size_t Element, typename Type, std::size_t Dimension>
class PointImpl<Element, Type, Dimension, std::enable_if_t<(Element >= Dimension), void>>
{
public:
  PointImpl() = default;
};

template <std::size_t Element, typename Type, std::size_t Dimension>
class PointImpl<Element, Type, Dimension, std::enable_if_t<(Element < Dimension), void>> : public PointImpl<Element + 1, Type, Dimension, void>
{
public:
  PointImpl() = default;
  // template <typename ...Args, std::enable_if_t<sizeof...(Args) == Dimension> = 0>
  // PointImpl(const Type valueElement, const Args... args) : PointImpl<Element + 1, Type, Dimension>(args...), _data(valueElement) { }

  template <std::enable_if_t<Element == 0, int> = 0>
  Type& x() { return _data; }
  template <std::enable_if_t<Element != 0, int> = 0>
  Type& x() { return _data; }
  // template <std::enable_if_t<Element == 1, int> = 0>
  // Type& y() { return _data; }
  // template <std::enable_if_t<Element == 2, int> = 0>
  // Type& z() { return _data; }


private:
  Type _data;
};

template <typename Type, std::size_t Dimension>
using Point = PointImpl<0, Type, Dimension, void>;

} // end namespace base

} // end namespace francor

// namespace std
// {

// inline ostream& operator<<(ostream& os, const francor::base::Point& angle)
// {
//   os << "angle [radian = " << angle.radian() << ", degree = " << angle.degree() << "]";

//   return os;
// }

// } // end namespace std