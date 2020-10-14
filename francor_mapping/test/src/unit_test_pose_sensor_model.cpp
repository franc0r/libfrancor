/**
 * Unit test for the kalman filter class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 08. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/pose_sensor_model.h"

using francor::mapping::PoseSensorModel;

TEST(PoseSensorModel, Instansitation)
{
  PoseSensorModel sensor_model;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}