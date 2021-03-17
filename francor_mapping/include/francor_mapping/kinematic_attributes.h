/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include <cstddef>

namespace francor {

namespace mapping {

enum class KinematicAttribute : std::uint8_t {
  POS_X = 0,
  POS_Y,
  VEL,
  VEL_X,
  VEL_Y,
  ACC,
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
 * \brief This template struct can hold a set of KinematicAttributes. It provides methods to check if an attributes is contained or not
 *        and to get the index of an attribute.
 */
template <std::size_t Index, KinematicAttribute... Attributes>
struct KinematicAttributePack;

/**
 * \brief Specialization for the top level struct (no state attribute is left).
 */
template <std::size_t Index>
struct KinematicAttributePack<Index>
{
  template <KinematicAttribute RequestAttribute>
  inline static constexpr bool hasAttribute() { /* returns false in case RequestedAttribute is in this vector */ return false; }

  inline static constexpr std::size_t count() { return Index; }
};

/**
 * \brief Specialization for core functionality. It is needed to seprate the first element of the expansion pack. This specialization
 *        actually also implement the methods hasAttributes() and getAttributeIndex().
 */
template <std::size_t Index, KinematicAttribute HeadAttribute, KinematicAttribute... TailAttributes>
struct KinematicAttributePack<Index, HeadAttribute, TailAttributes...> : public KinematicAttributePack<Index + 1, TailAttributes...>
{
protected:
  /**
   * \brief Counts the quantity of given attribute in this class and all parent classes.
   * \tparam Attribute Counts quantity of this attribute.
   * \return Quantity of given attribute.
   */
  template <KinematicAttribute Attribute>
  static constexpr std::size_t countQuantityOfAttribute()
  {
    std::size_t count = (Attribute == HeadAttribute ? 1 : 0);

    ( (Attribute == TailAttributes ? ++count : 0), ... );

    return count;
  }

  // Checks if the attribute of this class exists only once. It starts at index 0 and works like:
  // i0 == i1, i0 == i2, ..., i0 == in-1, i1 == i2, i1 == i3, ..., i1 == in-1, ..., in-1 == in-1
  static_assert(countQuantityOfAttribute<HeadAttribute>() == 1, "Each attribute must be exist only one time.");

public:
  /**
   * \brief Checks if a state attribute is contained.
   * 
   * \tparam TargetAttribute TargetAttribute is the attribute that will be search.
   * \return true if the TargetAttribute is contained.
   */
  template <KinematicAttribute TargetAttribute>
  inline static constexpr bool hasAttribute()
  {
    if constexpr (TargetAttribute == HeadAttribute) {
      return true;
    }
    else {
      return KinematicAttributePack<Index + 1, TailAttributes...>::template hasAttribute<TargetAttribute>();
    }
  }

  /**
   * \brief Returns the index of an attribute. If the attribute is not contained the compilation will fail, because
   *        of an static assert.
   * 
   * \tparam TargetAttribute The index of this attribute will be returned.
   * \return Index of the given attribute.
   */
  template <KinematicAttribute TargetAttribute>
  inline static constexpr std::size_t getAttributeIndex()
  {
    if constexpr (TargetAttribute == HeadAttribute) {
      return Index;
    }
    else {
      return KinematicAttributePack<Index + 1, TailAttributes...>::template getAttributeIndex<TargetAttribute>();
    }

    // assert if the requested attribute isn't contained in this vector
    static_assert((sizeof...(TailAttributes) == 0 && TargetAttribute == HeadAttribute)
                  ||
                  (sizeof...(TailAttributes) > 0),
                  "Requested index for the given attribute doesn't exists.");
  }

  /**
   * \brief Returns attribute of given index. Compilation fails if given index doesn't exist.
   * \tparam TargetIndex Index of wanted attribute.
   * \return Attribute of given index.
   */
  template <std::size_t TargetIndex>
  inline static constexpr KinematicAttribute getAttributeByIndex()
  {
    if constexpr (TargetIndex == Index) {
      return HeadAttribute;
    }
    else {
      return KinematicAttributePack<Index + 1, TailAttributes...>::template getAttributeByIndex<TargetIndex>();
    }

    // assert if target index is out of range
    static_assert((sizeof...(TailAttributes) == 0 && TargetIndex == Index)
                  ||
                  (sizeof...(TailAttributes) > 0),
                  "Given index is out of range.");
  }

  /**
   * \brief Return the quantity of attributes of this class and all parent classes.
   * \return Count of attributes.
   */
  static inline constexpr std::size_t count()
  {
    return KinematicAttributePack<Index + 1, TailAttributes...>::count();
  }
};

} // end namespace impl

// create kinematic attribute vector using an alias
template <KinematicAttribute... Attributes>
using KinematicAttributePack = impl::KinematicAttributePack<0, Attributes...>;

} // end namespace mapping

} // end namespace francor