/**
 * This file implements functionality for class KinematicStateVector.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include <francor_base/angle.h>

namespace francor {

namespace mapping {

namespace impl {

template <KinematicAttribute Attribute>
struct resolve_type { using type = double; };

template <> struct resolve_type<KinematicAttribute::ROLL>       { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::PITCH>      { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::YAW>        { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::ROLL_RATE>  { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::PITCH_RATE> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::YAW_RATE>   { using type = francor::base::NormalizedAngle; };

// @todo configure value for memory alignment properly
template <KinematicAttribute Attribute, typename DataType = typename resolve_type<Attribute>::type>
class alignas(16) KinematicStateVectorDataStoreage
{
protected:
  using type = DataType;

  DataType data{};
};

template <KinematicAttribute Attribute>
class KinematicStateVectorData : public KinematicStateVectorDataStoreage<Attribute> { };

template <>
class KinematicStateVectorData<KinematicAttribute::POS_X> : public KinematicStateVectorDataStoreage<KinematicAttribute::POS_X>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::POS_X>::type;

public:
  inline constexpr DataType& x() { return KinematicStateVectorDataStoreage<KinematicAttribute::POS_X>::data; }
  inline constexpr const DataType& x() const { return KinematicStateVectorDataStoreage<KinematicAttribute::POS_X>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::POS_Y> : public KinematicStateVectorDataStoreage<KinematicAttribute::POS_Y>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::POS_Y>::type;

public:
  inline constexpr DataType& y() { return KinematicStateVectorDataStoreage<KinematicAttribute::POS_Y>::data; }
  inline constexpr const DataType& y() const { return KinematicStateVectorDataStoreage<KinematicAttribute::POS_Y>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::ACC_X> : public KinematicStateVectorDataStoreage<KinematicAttribute::ACC_X>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::ACC_X>::type;

public:
  inline constexpr DataType& accelerationX() { return KinematicStateVectorDataStoreage<KinematicAttribute::ACC_X>::data; }
  inline constexpr const DataType& accelerationX() const { return KinematicStateVectorDataStoreage<KinematicAttribute::ACC_X>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::ACC_Y> : public KinematicStateVectorDataStoreage<KinematicAttribute::ACC_Y>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::ACC_Y>::type;

public:
  inline constexpr DataType& accelerationY() { return KinematicStateVectorDataStoreage<KinematicAttribute::ACC_Y>::data; }
  inline constexpr const DataType& accelerationY() const { return KinematicStateVectorDataStoreage<KinematicAttribute::ACC_Y>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::YAW> : public KinematicStateVectorDataStoreage<KinematicAttribute::YAW>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::YAW>::type;

public:
  inline constexpr DataType& yaw() { return KinematicStateVectorDataStoreage<KinematicAttribute::YAW>::data; }
  inline constexpr const DataType& yaw() const { return KinematicStateVectorDataStoreage<KinematicAttribute::YAW>::data; }
};

template <std::size_t Index, KinematicAttribute... Attributes>
class KinematicStateVectorValue;

template <std::size_t Index>
class KinematicStateVectorValue<Index> { };

template <std::size_t Index, KinematicAttribute HeadAttribute, KinematicAttribute... TailAttributes>
class KinematicStateVectorValue<Index, HeadAttribute, TailAttributes...> 
  : public KinematicStateVectorValue<Index + 1, TailAttributes...>
  , public KinematicStateVectorData<HeadAttribute>
{
public:
  template <KinematicAttribute Attribute>
  inline constexpr const typename resolve_type<Attribute>::type& value() const
  {
    if constexpr (Attribute == HeadAttribute) {
      return KinematicStateVectorData<HeadAttribute>::data;
    }
    else {
      return KinematicStateVectorValue<Index + 1, TailAttributes...>::template value<Attribute>();
    }
  }
  template <KinematicAttribute Attribute>
  inline constexpr typename resolve_type<Attribute>::type& value()
  {
    if constexpr (Attribute == HeadAttribute) {
      return KinematicStateVectorData<HeadAttribute>::data;
    }
    else {
      return KinematicStateVectorValue<Index + 1, TailAttributes...>::template value<Attribute>();
    }
  }
};

} // end namespace impl

template <KinematicAttribute... Attributes>
using KinematicStateVectorValue = impl::KinematicStateVectorValue<0, Attributes...>;

} // end namespace mapping

} // end namespace francor