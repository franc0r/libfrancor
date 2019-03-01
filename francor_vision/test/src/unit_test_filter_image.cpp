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

// helper image filter classes
class DummyFilter : public ImageFilter
{
protected:
  DummyFilter(const ColourSpace space) : ImageFilter(space) { }

public:
  virtual bool isValid(void) const override { return true; }
};

class DummyFilterRgb : public DummyFilter
{
public:
  DummyFilterRgb(const bool valid = true) : DummyFilter(ColourSpace::RGB), is_valid_(valid) { }

  virtual bool isValid(void) const override { return is_valid_; }

private:
  virtual bool processImpl(Image& image) const override
  {
    if (image.rows() == 0 || image.cols() == 0)
      return false;

    image(0, 0).r() = 10;
    image(0, 0).g() = 20;
    image(0, 0).b() = 30;

    return true;
  }

  const bool is_valid_;
};

class DummyMaskFilter : public ImageMaskFilter
{
protected:
  DummyMaskFilter(const ColourSpace space) : ImageMaskFilter(space) { }

public:
  virtual bool isValid(void) const override { return true; }
};

class DummyMaskFilterRgb : public DummyMaskFilter
{
public:
  DummyMaskFilterRgb(const bool valid = true, const std::size_t row = 0, const std::size_t col = 0) : DummyMaskFilter(ColourSpace::RGB), is_valid_(valid), row_(row), col_(col) { }

  virtual bool isValid(void) const override { return is_valid_; }

private:
  virtual bool processImpl(const Image& image, Image& mask) const override
  {
    if (image.rows() <= row_ || image.cols() <= col_ || mask.cols() <= col_ || mask.rows() <= row_)
      return false;

    mask(row_, col_).bit() = image(row_, col_).r() >= 50 && image(row_, col_).g() >= 50 && image(row_, col_).b() >= 50;

    return true;
  }

  const bool is_valid_;
  const std::size_t row_;
  const std::size_t col_;
};

TEST(ImageFilterPipelineTest, instantiateEmptyPipeline)
{
  francor::vision::ImageFilterPipeline pipeline;

  EXPECT_TRUE(pipeline.isValid());
}

TEST(ImageFilterPipelineTest, AddFilter)
{
  francor::vision::ImageFilterPipeline pipeline;

  // add a not valid filter
  ASSERT_FALSE(pipeline.addFilter("rgb", std::make_unique<DummyFilterRgb>(false)));
  ASSERT_EQ(pipeline.numOfFilters(), 0);

  // add a valid filter
  ASSERT_TRUE(pipeline.addFilter("rgb", std::make_unique<DummyFilterRgb>(true)));
  ASSERT_EQ(pipeline.numOfFilters(), 1);

  // use the filter name twice
  ASSERT_FALSE(pipeline.addFilter("rgb", std::make_unique<DummyFilterRgb>(true)));
  ASSERT_EQ(pipeline.numOfFilters(), 1);

  EXPECT_TRUE(pipeline.isValid());
}

TEST(ImageFilterPipelineTest, Process)
{
  francor::vision::ImageFilterPipeline pipeline;
  Image image(Image::zeros(10, 10, ColourSpace::RGB));

  // pipeline with no filters shouldn't fail
  EXPECT_TRUE(pipeline(image));

  // add a valid filter
  ASSERT_TRUE(pipeline.addFilter("rgb", std::make_unique<DummyFilterRgb>(true)));
  ASSERT_EQ(pipeline.numOfFilters(), 1);

  // normal operation without expected error
  EXPECT_TRUE(pipeline(image));

  // checks only if the image was modified, not that the filter works properly
  EXPECT_EQ(image(0, 0).r(), 10);
  EXPECT_EQ(image(0, 0).g(), 20);
  EXPECT_EQ(image(0, 0).b(), 30);

  // clear image and expect failing pipeline
  image.clear();
  
  EXPECT_FALSE(pipeline(image));
}

TEST(ImageMaskFilterPipeLineTest, Process)
{
  francor::vision::ImageMaskFilterPipeline pipeline;
  Image image(Image::zeros(10, 10, ColourSpace::RGB));
  Image mask;

  // the pipeline must not fail without any filter
  EXPECT_TRUE(pipeline(image, mask));
  // the pipeline must create a bit mask according the size of image
  EXPECT_EQ(mask.rows(), image.rows());
  EXPECT_EQ(mask.cols(), image.cols());
  EXPECT_EQ(mask.colourSpace(), ColourSpace::BIT_MASK);

  // mask must be set to zero during initialization
  for (std::size_t row = 0; row < mask.rows(); ++row)
    for (std::size_t col = 0; col < mask.cols(); ++col)
      EXPECT_FALSE(mask(row, col).bit());

  // add a valid filter
  ASSERT_TRUE(pipeline.addFilter("rgb", std::make_unique<DummyMaskFilterRgb>(true)));
  ASSERT_EQ(pipeline.numOfFilters(), 1);

  // modify data so the mask will be modified
  image(0, 0).r() = 100;
  image(0, 0).g() = 100;
  image(0, 0).b() = 100;

  // normal operation without expected error
  EXPECT_TRUE(pipeline(image, mask));

  // checks only if the mask was modified, not that the filter works properly
  EXPECT_TRUE(mask(0, 0).bit());

  // clear image and expect failing pipeline, because filter can't work with that image type
  image.clear();
  
  EXPECT_FALSE(pipeline(image, mask));
}

TEST(ImageMaskFilterPipeLineTest, CombinedMaskFromTwoFilters)
{
  francor::vision::ImageMaskFilterPipeline pipeline;
  Image image(Image::zeros(10, 10, ColourSpace::RGB));
  Image mask;

  // add two valid filter
  ASSERT_TRUE(pipeline.addFilter("rgb_0", std::make_unique<DummyMaskFilterRgb>(true, 0, 0))); // checks pixel 0,0
  ASSERT_TRUE(pipeline.addFilter("rgb_1", std::make_unique<DummyMaskFilterRgb>(true, 0, 1))); // checks pixel 0,1
  ASSERT_EQ(pipeline.numOfFilters(), 2);

  // modify data so the mask will be modified
  image(0, 0).r() = 100;
  image(0, 0).g() = 100;
  image(0, 0).b() = 100;
  image(0, 1).r() = 100;
  image(0, 1).g() = 100;
  image(0, 1).b() = 100;

  // normal operation without expected error
  EXPECT_TRUE(pipeline(image, mask));

  // checks only if the mask was modified, not that the filter works properly
  EXPECT_GE(mask.rows(), 1);
  EXPECT_GE(mask.cols(), 2);
  EXPECT_TRUE(mask(0, 0).bit());
  EXPECT_TRUE(mask(0, 1).bit());
}

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
  ASSERT_TRUE(pipeline(image, mask));

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