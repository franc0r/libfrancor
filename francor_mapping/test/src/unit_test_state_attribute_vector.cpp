/**
 * Unit test for state attribute vector.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 03. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/kinematic_attributes.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicAttributePack;

// defines attribute vector that is used for the unit tests
using TestAttributeVector = KinematicAttributePack<KinematicAttribute::ACC_X,
                                                   KinematicAttribute::ACC_Y,
                                                   KinematicAttribute::POS_X,
                                                   KinematicAttribute::POS_Y>;

/**
 * \brief Checks the method hasAttribute() of KinematicAttributePack.
 */
TEST(KinematicAttributePack, hasAttribute)
{
  // first test positive requests
  EXPECT_TRUE(TestAttributeVector::hasAttribute<KinematicAttribute::ACC_X>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<KinematicAttribute::ACC_Y>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<KinematicAttribute::POS_X>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<KinematicAttribute::POS_Y>());

  // then all negative request
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::VEL_X>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::VEL_Y>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::ROLL>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::PITCH>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::YAW>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::ROLL_RATE>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::PITCH_RATE>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<KinematicAttribute::YAW_RATE>());
}

/**
 * \brief Checks the method getAttributeIndex() of KinematicAttributePack 
 *        using the defined TestAttributeVector.
 */
TEST(KinematicAttributePack, getAttributeIndex)
{
  EXPECT_EQ(0, TestAttributeVector::getAttributeIndex<KinematicAttribute::ACC_X>());
  EXPECT_EQ(1, TestAttributeVector::getAttributeIndex<KinematicAttribute::ACC_Y>());
  EXPECT_EQ(2, TestAttributeVector::getAttributeIndex<KinematicAttribute::POS_X>());
  EXPECT_EQ(3, TestAttributeVector::getAttributeIndex<KinematicAttribute::POS_Y>());

  // EXPECT_EQ(0, TestAttributeVector::getAttributeIndex<KinematicAttribute::VEL_X>());
  // compilation fails because of an static assert.
  // @todo override somehow static_assert for this test. Somehow it should be possible...
}

/**
 * \brief Checks the method getNumberOfAttributes() using defined TestAttributeVector.
 */
TEST(KinematicAttributePack, getNumberOfAttributes)
{
  EXPECT_EQ(4, TestAttributeVector::count());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}