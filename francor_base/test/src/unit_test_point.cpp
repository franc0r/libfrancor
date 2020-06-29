/**
 * Unit test for the point class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. October 2019
 */
#include <gtest/gtest.h>

#include "francor_base/point.h"

using francor::base::Point2d;

TEST(Point2d, Instantiate)
{
  Point2d point[2];

  for (int i = 0; i < 2; ++i) {
    std::cout << "point[" << i << "]: " << &point[i] << std::endl;
    std::cout << "x: " << &point[i].x() << std::endl;
    std::cout << "y: " << &point[i].y() << std::endl;
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}