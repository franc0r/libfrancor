/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include <cstddef>

namespace francor {

namespace mapping {

enum class StateAttribute : std::uint8_t {
  POS_X = 0,
  POS_Y,
  VEL_X,
  VEL_Y,
  ACC_X,
  ACC_Y,
  ROLL,
  PITCH,
  YAW,
  ROLL_RATE,
  PITCH_RATE,
  YAW_RATE,
};

namespace impl {

template <std::size_t Index, StateAttribute... Attributes>
struct StateAttributeVector;

template <std::size_t Index>
struct StateAttributeVector<Index>
{
  template <StateAttribute RequestAttribute>
  static constexpr bool hasAttribute() { /* returns false in case RequestedAttribute is in this vector */ return false; }

  // template <StateAttribute RequestedAttribute>
  // static constexpr std::size_t getAttributeIndex()
  // {
  //   return 
  // }
};

template <std::size_t Index, StateAttribute HeadAttribute, StateAttribute... TailAttributes>
struct StateAttributeVector<Index, HeadAttribute, TailAttributes...> : public StateAttributeVector<Index + 1, TailAttributes...>
{
  template <StateAttribute RequestAttribute>
  static constexpr bool hasAttribute()
  {
    if constexpr (RequestAttribute == HeadAttribute) {
      return true;
    }
    else {
      return StateAttributeVector<Index + 1, TailAttributes...>::template hasAttribute<RequestAttribute>();
    }
  }

  template <StateAttribute RequestedAttribute>
  static constexpr std::size_t getAttributeIndex()
  {
    if constexpr (RequestedAttribute == HeadAttribute) {
      return Index;
    }
    else {
      return StateAttributeVector<Index + 1, TailAttributes...>::template getAttributeIndex<RequestedAttribute>();
    }

    static_assert((sizeof...(TailAttributes) == 0 && RequestedAttribute == HeadAttribute)
                  ||
                  (sizeof...(TailAttributes) > 0),
                  "Requested index for the given attribute doesn't exists.");
  }
};

} // end namespace impl

template <StateAttribute... Attributes>
using StateAttributeVector = impl::StateAttributeVector<0, Attributes...>;

} // end namespace mapping

} // end namespace francor