/**
 * Unit test for the image filter classes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 1. March 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/image_filter_criteria.h"

using francor::vision::ImageMaskFilterColourRange;
using francor::vision::Image;
using francor::vision::ColourSpace;

TEST(ImageMaskFilterColourRangeTest, MissConfiguration)
{
  ImageMaskFilterColourRange filter(10, 0, 10, 0, 10, 0);

  EXPECT_FALSE(filter.isValid());
}

TEST(ImageMaskFilterColourRangeTest, Processing)
{
  ImageMaskFilterColourRange filter(0, 10, 20, 30, 40, 50);
  Image image(1, 2, ColourSpace::HSV);
  Image mask(image.rows(), image.cols(), ColourSpace::BIT_MASK);

  image(0, 0).h() =  5;
  image(0, 0).s() = 25;
  image(0, 0).v() = 45;

  image(0, 1).h() =  100;
  image(0, 1).s() =  100;
  image(0, 1).v() =  100;

  // filter must be valid
  ASSERT_TRUE(filter.isValid());
  // pixel 0,0 fulfill the filter criterion, pixel 0,1 doesn't 
  EXPECT_TRUE(filter.process(image, mask));

  EXPECT_GE(mask.rows(), 1);
  EXPECT_GE(mask.cols(), 2);
  EXPECT_TRUE(mask(0, 0).bit());
  EXPECT_FALSE(mask(0, 1).bit());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
