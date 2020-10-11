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
                                                        KinematicAttribute::VEL_X,
                                                        KinematicAttribute::VEL_Y,
                                                        KinematicAttribute::ACC_X,
                                                        KinematicAttribute::ACC_Y,
                                                        KinematicAttribute::ROLL,
                                                        KinematicAttribute::PITCH,
                                                        KinematicAttribute::YAW,
                                                        KinematicAttribute::ROLL_RATE,
                                                        KinematicAttribute::PITCH_RATE,
                                                        KinematicAttribute::YAW_RATE>;

class EgoKalmanFilterModel : public KalmanFilterModel<EgoModelKalmanAttributes>
{
public:
  EgoKalmanFilterModel();

  Matrix getPredictionMatrix(const double delta_time) const override;

private:
  
};

} // end namespace mapping

} // end namespace francor