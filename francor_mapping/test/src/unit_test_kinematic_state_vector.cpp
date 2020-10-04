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

using TestAttributes = KinematicAttributePack<KinematicAttribute::ACC_X,
                                              KinematicAttribute::ACC_Y,
                                              KinematicAttribute::POS_X,
                                              KinematicAttribute::POS_Y,
                                              KinematicAttribute::YAW>;

TEST(KinematicStateVector, Instantiate)
{
  TestAttributes state_vector;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}