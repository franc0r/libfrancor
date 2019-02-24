/**
 * Unit test for the image filter classes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 23. February 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/image_filter_pipeline.h"

TEST(ImageFilterPipelineTest, instantiateEmptyPipeline)
{
  francor::vision::ImageFilterPipeline pipeline;

  EXPECT_TRUE(pipeline.isValid());
}

using francor::vision::ImageFilter;
using francor::vision::Image;
using francor::vision::ColourSpace;

class DummyFilter : public ImageFilter
{
public:
  virtual bool isValid(void) const override { return true; }

protected:
  virtual bool processImpl(Image& image) const override { return true; }
  virtual bool processImpl(const Image& image, Image& mask) const override { return true; }
};

TEST(ImageFilterPipeLineTest, CreateRequiredImageTypes)
{
  // uses declarations and class definitions to check if the correct colour space is supplied for each filter
  static ColourSpace rgb = ColourSpace::NONE, bgr = ColourSpace::NONE, hsv = ColourSpace::NONE, gray = ColourSpace::NONE;

  class DummyFilterRgb  : public DummyFilter { virtual ColourSpace requiredColourSpace(void) const override { return ColourSpace::RGB;  }
                                               virtual bool processImpl(Image& image) const override { rgb = image.colourSpace(); return true; } };
  class DummyFilterBgr  : public DummyFilter { virtual ColourSpace requiredColourSpace(void) const override { return ColourSpace::BGR;  }
                                               virtual bool processImpl(Image& image) const override { bgr = image.colourSpace(); return true; } };
  class DummyFilterHsv  : public DummyFilter { virtual ColourSpace requiredColourSpace(void) const override { return ColourSpace::HSV;  }
                                               virtual bool processImpl(Image& image) const override { hsv = image.colourSpace(); return true; } };
  class DummyFilterGray : public DummyFilter { virtual ColourSpace requiredColourSpace(void) const override { return ColourSpace::GRAY; }
                                               virtual bool processImpl(Image& image) const override { gray = image.colourSpace(); return true; } };

  francor::vision::ImageFilterPipeline pipeline;

  ASSERT_TRUE(pipeline.addFilter("rgb" , std::make_unique<DummyFilterRgb >()));
  ASSERT_TRUE(pipeline.addFilter("bgr" , std::make_unique<DummyFilterBgr >()));
  ASSERT_TRUE(pipeline.addFilter("hsv" , std::make_unique<DummyFilterHsv >()));
  ASSERT_TRUE(pipeline.addFilter("gray", std::make_unique<DummyFilterGray>()));

  Image image(100, 100, ColourSpace::BGR);

  // check if correct colour space was supplied.
  EXPECT_TRUE(pipeline.process(image));

  EXPECT_EQ(rgb , ColourSpace::RGB );
  EXPECT_EQ(bgr , ColourSpace::BGR );
  EXPECT_EQ(hsv , ColourSpace::HSV );
  EXPECT_EQ(gray, ColourSpace::GRAY);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}