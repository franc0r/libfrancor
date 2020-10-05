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

using AllAttributes = KinematicAttributePack<KinematicAttribute::POS_X,
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

  // verify values stored in state_vector
  EXPECT_EQ(x, state_vector.x());
  EXPECT_EQ(y, state_vector.y());
  EXPECT_EQ(acceleration_x, state_vector.accelerationX());
  EXPECT_EQ(acceleration_y, state_vector.accelerationY());
  EXPECT_EQ(yaw, state_vector.yaw());
}

TEST(KinematicStateVector, CheckMemoryAlignment)
{
  const KinematicStateVector<AllAttributes> status_vector;
  std::array<const std::uint8_t*, status_vector.count()> data_addresses = { 0 };

  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::POS_X>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.x());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::POS_Y>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.y());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::VEL_X>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.velocityX());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::VEL_Y>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.velocityY());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::ACC_X>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.accelerationX());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::ACC_Y>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.accelerationY());

  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::ROLL>()]       = reinterpret_cast<const std::uint8_t*>(&status_vector.roll());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::PITCH>()]      = reinterpret_cast<const std::uint8_t*>(&status_vector.pitch());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::YAW>()]        = reinterpret_cast<const std::uint8_t*>(&status_vector.yaw());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::ROLL_RATE>()]  = reinterpret_cast<const std::uint8_t*>(&status_vector.rollRate());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::PITCH_RATE>()] = reinterpret_cast<const std::uint8_t*>(&status_vector.pitchRate());
  data_addresses[status_vector.getAttributeIndex<KinematicAttribute::YAW_RATE>()]   = reinterpret_cast<const std::uint8_t*>(&status_vector.yawRate());

  std::array<std::uint64_t, data_addresses.size() - 1> address_differences = { 0 };

  for (std::size_t i = 1; i < data_addresses.size(); ++i) {
    address_differences[i - 1] = data_addresses[i - 1] - data_addresses[i];
  }

  for (const auto diff : address_differences) {
    EXPECT_EQ(address_differences[0], diff);
  }

  // @todo replace magic number
  EXPECT_EQ(16, address_differences[0]);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}