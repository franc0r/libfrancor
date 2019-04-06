/**
 * Unit test for input and output data operations.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/io.h"

using francor::vision::loadImageFromFile;
using francor::vision::Image;
using francor::vision::ColourSpace;

TEST(loadImageFromFile, LoadColouredImage)
{
  Image image(std::move(loadImageFromFile("/home/knueppl/repos/francor/libfrancor/francor_vision/test/testdata/bgr.png", ColourSpace::BGR)));

  ASSERT_EQ(image.cols(), 10);
  ASSERT_EQ(image.rows(), 10);

  EXPECT_EQ(image(0, 0).r(), 255);
  EXPECT_EQ(image(0, 0).g(),   0);
  EXPECT_EQ(image(0, 0).b(),   0);

  EXPECT_EQ(image(9, 9).r(),   0);
  EXPECT_EQ(image(9, 9).g(),   0);
  EXPECT_EQ(image(9, 9).b(), 255);

  EXPECT_EQ(image(5, 5).r(),   0);
  EXPECT_EQ(image(5, 5).g(), 255);
  EXPECT_EQ(image(5, 5).b(),   0);
}

TEST(loadImageFromFile, LoadGrayImage)
{
  Image image(std::move(loadImageFromFile("/home/knueppl/repos/francor/libfrancor/francor_vision/test/testdata/bgr.png", ColourSpace::GRAY)));

  ASSERT_EQ(image.cols(), 10);
  ASSERT_EQ(image.rows(), 10);

  EXPECT_GT(image(0, 0).gray(), 0);
  EXPECT_GT(image(0, 0).gray(), 0);
  EXPECT_EQ(image(3, 3).gray(), 0);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
