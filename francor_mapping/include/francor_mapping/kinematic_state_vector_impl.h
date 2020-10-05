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
template <KinematicAttribute Attribute, std::size_t DataAlignment = 16, typename DataType = typename resolve_type<Attribute>::type>
class alignas(DataAlignment) KinematicStateVectorDataStoreage
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
class KinematicStateVectorData<KinematicAttribute::VEL_X> : public KinematicStateVectorDataStoreage<KinematicAttribute::VEL_X>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::VEL_X>::type;

public:
  inline constexpr DataType& velocityX() { return KinematicStateVectorDataStoreage<KinematicAttribute::VEL_X>::data; }
  inline constexpr const DataType& velocityX() const { return KinematicStateVectorDataStoreage<KinematicAttribute::VEL_X>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::VEL_Y> : public KinematicStateVectorDataStoreage<KinematicAttribute::VEL_Y>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::VEL_Y>::type;

public:
  inline constexpr DataType& velocityY() { return KinematicStateVectorDataStoreage<KinematicAttribute::VEL_Y>::data; }
  inline constexpr const DataType& velocityY() const { return KinematicStateVectorDataStoreage<KinematicAttribute::VEL_Y>::data; }
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

template <>
class KinematicStateVectorData<KinematicAttribute::ROLL> : public KinematicStateVectorDataStoreage<KinematicAttribute::ROLL>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::ROLL>::type;

public:
  inline constexpr DataType& roll() { return KinematicStateVectorDataStoreage<KinematicAttribute::ROLL>::data; }
  inline constexpr const DataType& roll() const { return KinematicStateVectorDataStoreage<KinematicAttribute::ROLL>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::PITCH> : public KinematicStateVectorDataStoreage<KinematicAttribute::PITCH>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::PITCH>::type;

public:
  inline constexpr DataType& pitch() { return KinematicStateVectorDataStoreage<KinematicAttribute::PITCH>::data; }
  inline constexpr const DataType& pitch() const { return KinematicStateVectorDataStoreage<KinematicAttribute::PITCH>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::ROLL_RATE> : public KinematicStateVectorDataStoreage<KinematicAttribute::ROLL_RATE>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::ROLL_RATE>::type;

public:
  inline constexpr DataType& rollRate() { return KinematicStateVectorDataStoreage<KinematicAttribute::ROLL_RATE>::data; }
  inline constexpr const DataType& rollRate() const { return KinematicStateVectorDataStoreage<KinematicAttribute::ROLL_RATE>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::PITCH_RATE> : public KinematicStateVectorDataStoreage<KinematicAttribute::PITCH_RATE>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::PITCH_RATE>::type;

public:
  inline constexpr DataType& pitchRate() { return KinematicStateVectorDataStoreage<KinematicAttribute::PITCH_RATE>::data; }
  inline constexpr const DataType& pitchRate() const { return KinematicStateVectorDataStoreage<KinematicAttribute::PITCH_RATE>::data; }
};

template <>
class KinematicStateVectorData<KinematicAttribute::YAW_RATE> : public KinematicStateVectorDataStoreage<KinematicAttribute::YAW_RATE>
{
  using DataType = typename KinematicStateVectorDataStoreage<KinematicAttribute::YAW_RATE>::type;

public:
  inline constexpr DataType& yawRate() { return KinematicStateVectorDataStoreage<KinematicAttribute::YAW_RATE>::data; }
  inline constexpr const DataType& yawRate() const { return KinematicStateVectorDataStoreage<KinematicAttribute::YAW_RATE>::data; }
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