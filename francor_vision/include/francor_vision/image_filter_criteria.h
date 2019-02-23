/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include "francor_vision/filter_image_pipeline.h"

namespace francor {

namespace base {

class ImageFilterColour : public ImageFilter
{
public:
  ImageFilterColour(void) = delete;
  ImageFilterColour(const std::uint8_t minH,
                        const std::uint8_t maxH,
                        const std::uint8_t minS,
                        const std::uint8_t maxS,
                        const std::uint8_t minV,
                        const std::uint8_t maxV)
    : min_h_(minH),
      max_h_(maxH),
      min_s_(minS),
      max_s_(maxS),
      min_v_(minV),
      max_v_(maxV)
  {

  }

  virtual bool processImpl(Image& image) const override
  {
    Image mask(image.rows(), image.cols(), ColourSpace::BIT_MASK);
    bool ret = this->processImpl(image, mask);
    image.applyMask(mask);

    return ret;
  }

  virtual bool processImpl(const Image& image, Image& mask) const override
  {
    // HSV colour space is required
    if (image.colourSpace() != ColourSpace::HSV)
    {
      return false;
    }

    cv::Mat tempMask(mask.cvMat());
    cv::Mat resultMask;

    cv::inRange(image.cvMat(), cv::Scalar(min_h_, min_s_, min_v_), cv::Scalar(max_h_, max_s_, max_v_), resultMask);
    cv::bitwise_or(tempMask, resultMask, tempMask);

    return true;
  }

  virtual ColourSpace requiredColourSpace(void) const override { return ColourSpace::HSV; }
  virtual bool isValid(void) const override { return min_h_ <= max_h_ && min_s_ <= max_s_ && min_v_ <= max_v_; }

private:
  const std::uint8_t min_h_;
  const std::uint8_t max_h_;
  const std::uint8_t min_s_;
  const std::uint8_t max_s_;
  const std::uint8_t min_v_;
  const std::uint8_t max_v_;
};

} // end namespace perception

} // end namespace francor