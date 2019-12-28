/**
 * Unit test for the math functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 28. December 2019
 */
#include <gtest/gtest.h>

#include "francor_base/algorithm/math.h"

TEST(Fak, Functionality)
{
  EXPECT_EQ(francor::base::algorithm::math::fak(4), 24);
  EXPECT_EQ((francor::base::algorithm::math::fak<4, int>()), 24);

  EXPECT_EQ(francor::base::algorithm::math::fak(10), 3628800);
  EXPECT_EQ((francor::base::algorithm::math::fak<10, int>()), 3628800);

}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}