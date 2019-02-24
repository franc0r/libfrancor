/**
 * Unit test for the image filter classes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 23. February 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/image_filter_pipeline.h"

using francor::vision::ImageFilter;
using francor::vision::ImageMaskFilter;
using francor::vision::Image;
using francor::vision::ColourSpace;

class DummyFilter : public ImageFilter
{
protected:
  DummyFilter(const ColourSpace space) : ImageFilter(space) { }

public:
  virtual bool isValid(void) const override { return true; }
};

class DummyMaskFilter : public ImageMaskFilter
{
protected:
  DummyMaskFilter(const ColourSpace space) : ImageMaskFilter(space) { }

public:
  virtual bool isValid(void) const override { return true; }
};



TEST(ImageFilterPipelineTest, instantiateEmptyPipeline)
{
  francor::vision::ImageFilterPipeline pipeline;

  EXPECT_TRUE(pipeline.isValid());
}

// TEST(ImageFilterPipelineTest, AddFilter)
// {

// }

// TEST(ImageFilterPipelineTest, Process)
// {

// }

// TEST(ImageMaskFilterPipeLineTest, Process)
// {

// }

// TEST(ImageMaskFilterPipeLineTest, CombinedMaskFromTwoFilters)
// {

// }

TEST(ImageMaskFilterPipeLineTest, CreateRequiredImageTypes)
{
  // uses declarations and class definitions to check if the correct colour space is supplied for each filter
  static ColourSpace rgb = ColourSpace::NONE, bgr = ColourSpace::NONE, hsv = ColourSpace::NONE, gray = ColourSpace::NONE;

  class DummyMaskFilterRgb  : public DummyMaskFilter
  {
  public:
    DummyMaskFilterRgb(void) : DummyMaskFilter(ColourSpace::RGB) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { rgb = image.colourSpace(); return true; }
  };
  class DummyMaskFilterBgr  : public DummyMaskFilter
  {
  public:
    DummyMaskFilterBgr(void) : DummyMaskFilter(ColourSpace::BGR) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { bgr = image.colourSpace(); return true; }
  };
  class DummyMaskFilterHsv  : public DummyMaskFilter
  {
  public:
    DummyMaskFilterHsv(void) : DummyMaskFilter(ColourSpace::HSV) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { hsv = image.colourSpace(); return true; }
  };
  class DummyMaskFilterGray  : public DummyMaskFilter
  {
  public:
    DummyMaskFilterGray(void) : DummyMaskFilter(ColourSpace::GRAY) { }
    virtual bool processImpl(const Image& image, Image& mask) const override { gray = image.colourSpace(); return true; }
  };

  francor::vision::ImageMaskFilterPipeline pipeline;

  ASSERT_TRUE(pipeline.addFilter("rgb" , std::make_unique<DummyMaskFilterRgb >()));
  ASSERT_TRUE(pipeline.addFilter("bgr" , std::make_unique<DummyMaskFilterBgr >()));
  ASSERT_TRUE(pipeline.addFilter("hsv" , std::make_unique<DummyMaskFilterHsv >()));
  ASSERT_TRUE(pipeline.addFilter("gray", std::make_unique<DummyMaskFilterGray>()));

  Image image(10, 10, ColourSpace::BGR);
  Image mask;

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