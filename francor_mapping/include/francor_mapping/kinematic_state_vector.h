/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include <francor_base/angle.h>

#include "francor_mapping/kinematic_attributes.h"

namespace francor {

namespace mapping {

namespace impl {

template <KinematicAttribute Attribute>
struct resolve_type { typedef double type; };

template <> struct resolve_type<KinematicAttribute::ROLL> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::PITCH> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::YAW> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::ROLL_RATE> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::PITCH_RATE> { using type = francor::base::NormalizedAngle; };
template <> struct resolve_type<KinematicAttribute::YAW_RATE> { using type = francor::base::NormalizedAngle; };

// @todo configure value for memory alignment properly
template <KinematicAttribute Attribute, typename DataType = typename resolve_type<Attribute>::type>
struct alignas(16) KinematicStateVectorData
{
protected:
  DataType data{};
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

template <class AttributePack>
class KinematicStateVector;

template <KinematicAttribute... Attributes>
class KinematicStateVector<KinematicAttributePack<Attributes...>> : public KinematicAttributePack<Attributes...>
{
public:
  template <KinematicAttribute Attribute, typename DataType = typename impl::resolve_type<Attribute>::type>
  inline constexpr const DataType& operator()() const { return _values.template value<Attribute>(); }
  template <KinematicAttribute Attribute, typename DataType = typename impl::resolve_type<Attribute>::type>
  inline constexpr DataType& operator()() { return _values.template value<Attribute>(); }

private:
  impl::KinematicStateVectorValue<0, Attributes...> _values;
};



} // end namespace mapping

} // end namespace francor