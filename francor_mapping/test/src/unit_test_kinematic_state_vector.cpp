/**
 * Unit test for kinematic state vector.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 03. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/kinematic_state_vector.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicAttributePack;
using francor::mapping::KinematicStateVector;
using francor::base::NormalizedAngle;

using TestAttributes = KinematicAttributePack<KinematicAttribute::ACC_X,
                                              KinematicAttribute::ACC_Y,
                                              KinematicAttribute::POS_X,
                                              KinematicAttribute::POS_Y,
                                              KinematicAttribute::YAW>;

TEST(KinematicStateVector, Instantiate)
{
  KinematicStateVector<TestAttributes> state_vector;

  // check if following methods are created
  state_vector.x();
  state_vector.y();
  state_vector.accelerationX();
  state_vector.accelerationY();
  state_vector.yaw();
}

TEST(KinematicStateVector, SetAndGetAttributeValues)
{
  constexpr double x = 1.0;
  constexpr double y = 2.0;
  constexpr double acceleration_x = 3.0;
  constexpr double acceleration_y = 4.0;
  constexpr NormalizedAngle yaw(NormalizedAngle::createFromDegree(50.0));

  KinematicStateVector<TestAttributes> state_vector;

  // assign test values to attributes
  state_vector.x() = x;
  state_vector.y() = y;
  state_vector.accelerationX() = acceleration_x;
  state_vector.accelerationY() = acceleration_y;
  state_vector.yaw() = yaw;

  EXPECT_EQ(x, state_vector.x());
  EXPECT_EQ(y, state_vector.y());
  EXPECT_EQ(acceleration_x, state_vector.accelerationX());
  EXPECT_EQ(acceleration_y, state_vector.accelerationY());
  EXPECT_EQ(yaw, state_vector.yaw());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}