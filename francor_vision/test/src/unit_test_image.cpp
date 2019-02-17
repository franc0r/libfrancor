/**
 * Unit test for the image class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/image.h"

TEST(ImageTest, ConstructEmptyImage)
{
  francor::vision::Image image;

  EXPECT_EQ(image.cols, 0);
  EXPECT_EQ(image.rows, 0);
  EXPECT_EQ(image.colourSpace(), francor::vision::Image::ColourSpace::NONE);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}