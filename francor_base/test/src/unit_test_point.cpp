/**
 * Unit test for the point class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. October 2019
 */
#include <gtest/gtest.h>

#include "francor_base/point.h"

using francor::base::Point;

TEST(Point2d, Instantiate)
{
  // Point<double, 2> point;

  // point.x();
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}