/**
 * This file defines a kinematic state vector that is constructed by given attributes.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */
#pragma once

#include "francor_mapping/kinematic_attributes.h"

#include "francor_mapping/kinematic_state_vector_impl.h"

namespace francor {

namespace mapping {

template <class AttributePack>
class KinematicStateVector;

template <KinematicAttribute... Attributes>
class KinematicStateVector<KinematicAttributePack<Attributes...>>
  : public KinematicAttributePack<Attributes...>
  , public KinematicStateVectorValue<Attributes...>
{
public:

};

} // end namespace mapping

} // end namespace francor