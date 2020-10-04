/**
 * Unit test for state attribute vector.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 03. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/state_attributes.h"

using francor::mapping::StateAttribute;
using francor::mapping::StateAttributeVector;

// defines attribute vector that is used for the unit tests
using TestAttributeVector = StateAttributeVector<StateAttribute::ACC_X,
                                                 StateAttribute::ACC_Y,
                                                 StateAttribute::POS_X,
                                                 StateAttribute::POS_Y>;

/**
 * \brief Checks the method hasAttribute() of StateAttributeVector.
 */
TEST(StateAttributeVector, hasAttribute)
{
  // first test positive requests
  EXPECT_TRUE(TestAttributeVector::hasAttribute<StateAttribute::ACC_X>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<StateAttribute::ACC_Y>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<StateAttribute::POS_X>());
  EXPECT_TRUE(TestAttributeVector::hasAttribute<StateAttribute::POS_Y>());

  // then all negative request
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::VEL_X>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::VEL_Y>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::ROLL>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::PITCH>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::YAW>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::ROLL_RATE>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::PITCH_RATE>());
  EXPECT_FALSE(TestAttributeVector::hasAttribute<StateAttribute::YAW_RATE>());
}

/**
 * \brief Checks the method getAttributeIndex() of StateAttributeVector 
 *        using the defined TestAttributeVector.
 */
TEST(StateAttributeVector, getAttributeIndex)
{
  EXPECT_EQ(0, TestAttributeVector::getAttributeIndex<StateAttribute::ACC_X>());
  EXPECT_EQ(1, TestAttributeVector::getAttributeIndex<StateAttribute::ACC_Y>());
  EXPECT_EQ(2, TestAttributeVector::getAttributeIndex<StateAttribute::POS_X>());
  EXPECT_EQ(3, TestAttributeVector::getAttributeIndex<StateAttribute::POS_Y>());

  // EXPECT_EQ(0, TestAttributeVector::getAttributeIndex<StateAttribute::VEL_X>());
  // compilation fails because of an static assert.
  // @todo override somehow static_assert for this test. Somehow it should be possible...
}

/**
 * \brief Checks the method getNumberOfAttributes() using defined TestAttributeVector.
 */
TEST(StateAttributeVector, getNumberOfAttributes)
{
  EXPECT_EQ(4, TestAttributeVector::count());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}