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

/**
 * \brief This template struct can hold a set of StateAttributes. It provides methods to check if an attributes is contained or not
 *        and to get the index of an attribute.
 */
template <std::size_t Index, StateAttribute... Attributes>
struct StateAttributeVector;

/**
 * \brief Specialization for the top level struct (no state attribute is left).
 */
template <std::size_t Index>
struct StateAttributeVector<Index>
{
  template <StateAttribute RequestAttribute>
  static constexpr bool hasAttribute() { /* returns false in case RequestedAttribute is in this vector */ return false; }

  static inline constexpr std::size_t count() { return Index; }
};

/**
 * \brief Specialization for core functionality. It is needed to seprate the first element of the expansion pack. This specialization
 *        actually also implement the methods hasAttributes() and getAttributeIndex().
 */
template <std::size_t Index, StateAttribute HeadAttribute, StateAttribute... TailAttributes>
struct StateAttributeVector<Index, HeadAttribute, TailAttributes...> : public StateAttributeVector<Index + 1, TailAttributes...>
{
  /**
   * \brief Checks if a state attribute is contained.
   * 
   * \tparam TargetAttribute TargetAttribute is the attribute that will be search.
   * \return true if the TargetAttribute is contained.
   */
  template <StateAttribute TargetAttribute>
  static constexpr bool hasAttribute()
  {
    if constexpr (TargetAttribute == HeadAttribute) {
      return true;
    }
    else {
      return StateAttributeVector<Index + 1, TailAttributes...>::template hasAttribute<TargetAttribute>();
    }
  }

  /**
   * \brief Returns the index of an attribute. If the attribute is not contained the compilation will fail, because
   *        of an static assert.
   * 
   * \tparam TargetAttribute The index of this attribute will be returned.
   * \return Index of the given attribute.
   */
  template <StateAttribute TargetAttribute>
  static constexpr std::size_t getAttributeIndex()
  {
    if constexpr (TargetAttribute == HeadAttribute) {
      return Index;
    }
    else {
      return StateAttributeVector<Index + 1, TailAttributes...>::template getAttributeIndex<TargetAttribute>();
    }

    // assert if the requested attribute isn't contained in this vector
    static_assert((sizeof...(TailAttributes) == 0 && TargetAttribute == HeadAttribute)
                  ||
                  (sizeof...(TailAttributes) > 0),
                  "Requested index for the given attribute doesn't exists.");
  }

  template <std::size_t TargetIndex>
  static constexpr StateAttribute getAttributeByIndex()
  {
    if constexpr (TargetIndex == Index) {
      return HeadAttribute;
    }
    else {
      return StateAttributeVector<Index + 1, TailAttributes...>::template getAttributeByIndex<TargetIndex>();
    }

    // assert if target index is out of range
    static_assert((sizeof...(TailAttributes) == 0 && TargetIndex == Index)
                  ||
                  (sizeof...(TailAttributes) > 0),
                  "Given index is out of range.");
  }

  static inline constexpr std::size_t count()
  {
    return StateAttributeVector<Index + 1, TailAttributes...>::count();
  }
};

} // end namespace impl

// create state attribute vector using an alias
template <StateAttribute... Attributes>
using StateAttributeVector = impl::StateAttributeVector<0, Attributes...>;

} // end namespace mapping

} // end namespace francor