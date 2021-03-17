/**
 * This file defines a movement model for a ego vehicle using a kalman filter.
 * \author Christian Wendt (knueppl@gmx.de)
 * \date 3. October 2020
 */

#pragma once

#include "francor_mapping/kalman_filter_model.h"

namespace francor {

namespace mapping {

using EgoModelKalmanAttributes = KinematicAttributePack<KinematicAttribute::POS_X,
                                                        KinematicAttribute::POS_Y,
                                                        KinematicAttribute::VEL,
                                                        KinematicAttribute::ACC,
                                                        KinematicAttribute::YAW,
                                                        KinematicAttribute::YAW_RATE>;

class EgoKalmanFilterModel : public KalmanFilterModel<EgoModelKalmanAttributes>
{
public:

  Matrix getPredictionMatrix(const StateVector& current_state, const double delta_time) const override;
  Matrix getSystemNoiseMatrix(const StateVector& current_state, const double delta_time) const override;
};

} // end namespace mapping

} // end namespace francor