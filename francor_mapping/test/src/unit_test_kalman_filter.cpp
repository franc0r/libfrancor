/**
 * Unit test for the kalman filter class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 08. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicStateVector;
using francor::mapping::KalmanFilter;
using francor::mapping::EgoKalmanFilterModel;

TEST(KalmanFilter, Instansitate)
{
  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}