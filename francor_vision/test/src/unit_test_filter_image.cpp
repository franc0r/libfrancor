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

using francor::vision::ImageMaskFilter;
using francor::vision::Image;
using francor::vision::ColourSpace;

class DummyFilter : public ImageMaskFilter
{
protected:
  DummyFilter(const ColourSpace space) : ImageMaskFilter(space) { }

public:
  virtual bool isValid(void) const override { return true; }
};

TEST(ImageFilterPipeLineTest, CreateRequiredImageTypes)
{
  // uses declarations and class definitions to check if the correct colour space is supplied for each filter
  static ColourSpace rgb = ColourSpace::NONE, bgr = ColourSpace::NONE, hsv = ColourSpace::NONE, gray = ColourSpace::NONE;

  class DummyFilterRgb  : public DummyFilter
  {
  public:
    DummyFilterRgb(void) : DummyFilter(ColourSpace::RGB) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { rgb = image.colourSpace(); return true; }
  };
  class DummyFilterBgr  : public DummyFilter
  {
  public:
    DummyFilterBgr(void) : DummyFilter(ColourSpace::BGR) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { bgr = image.colourSpace(); return true; }
  };
  class DummyFilterHsv  : public DummyFilter
  {
  public:
    DummyFilterHsv(void) : DummyFilter(ColourSpace::HSV) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { hsv = image.colourSpace(); return true; }
  };
  class DummyFilterGray  : public DummyFilter
  {
  public:
    DummyFilterGray(void) : DummyFilter(ColourSpace::GRAY) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { gray = image.colourSpace(); return true; }
  };

  francor::vision::ImageMaskFilterPipeline pipeline;
  Image mask;

  ASSERT_TRUE(pipeline.addFilter("rgb" , std::make_unique<DummyFilterRgb >()));
  ASSERT_TRUE(pipeline.addFilter("bgr" , std::make_unique<DummyFilterBgr >()));
  ASSERT_TRUE(pipeline.addFilter("hsv" , std::make_unique<DummyFilterHsv >()));
  ASSERT_TRUE(pipeline.addFilter("gray", std::make_unique<DummyFilterGray>()));

  Image image(100, 100, ColourSpace::BGR);

  // check if correct colour space was supplied.
  ASSERT_TRUE(pipeline.process(image, mask));

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