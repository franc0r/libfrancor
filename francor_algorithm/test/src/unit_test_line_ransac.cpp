/**
 * Unit test for the line ransac class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 24. March 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/ransac.h"

using francor::base::VectorVector2d;
using francor::algorithm::LineRansac;

TEST(LineRansac, Instantiate)
{
  LineRansac ransac;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
