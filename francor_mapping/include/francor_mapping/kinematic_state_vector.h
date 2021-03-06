/**
 * This file defines a kinematic state vector that is constructed by given attributes.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include <francor_base/vector.h>

#include "francor_mapping/kinematic_attributes.h"

namespace francor {

namespace mapping {

namespace impl {

template <KinematicAttribute Attribute>
struct resolve_type { using type = double; };

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

template <class AttributePack>
class KinematicStateVector;

template <KinematicAttribute... Attributes>
class KinematicStateVector<KinematicAttributePack<Attributes...>>
  : public KinematicAttributePack<Attributes...>
  , public KinematicStateVectorValue<Attributes...>
{
public:
  static constexpr std::size_t size = KinematicAttributePack<Attributes...>::count();
  using type = double;
  using Vector = francor::base::VectorX<type, size>;

  KinematicStateVector() = default;
  explicit KinematicStateVector(const Vector& state_vector)
  {
    operator=(state_vector);
  }

  operator Vector() const
  {
    base::VectorX<type, size> data;
    std::size_t i = 0;
    ((data[i++] = static_cast<type>(this->template value<Attributes>())), ...);

    return data;
  }

  KinematicStateVector<KinematicAttributePack<Attributes...>>& operator=(const Vector& rhs)
  {
    std::size_t i = 0;
    ((this->template value<Attributes>() = rhs[i++]), ...);
    return *this;
  }
  KinematicStateVector<KinematicAttributePack<Attributes...>>
  operator-(const KinematicStateVector<KinematicAttributePack<Attributes...>>& rhs) const
  {
    KinematicStateVector<KinematicAttributePack<Attributes...>> result;
    ((result.template value<Attributes>() = this->template value<Attributes>() - rhs.template value<Attributes>()), ...);
    return result;
  }
};

} // end namespace mapping

} // end namespace francor

#include "francor_mapping/kinematic_state_vector_specialization.h"